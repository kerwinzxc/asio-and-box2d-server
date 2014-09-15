#include "stdafx.h"
#include "room.h"
#include "gameuser_machine.h"
#include "staticobject_machine.h"
#include "dumbmob_machine.h"

#include "packet_data.h"
#include "packet_encoder.h"

#include "util_makeindex.h"
#include "user_session.h"

room::room(boost::asio::io_service& arg_io)
	:m_timer(arg_io)
	, m_strand(arg_io)
{
}


room::~room()
{

}

void room::start()
{
	b2Vec2 grivity(0.0f, -10.0f);
	m_world = boost::make_shared<b2World>(std::move(grivity));
	m_world->SetContactListener(this);
	m_makeindex = boost::make_shared<util_makeindex>();


	m_timer.expires_from_now(boost::posix_time::milliseconds(66));
	m_timer.async_wait(m_strand.wrap(boost::bind(&room::update, shared_from_this())));

	m_pretime = m_timer.expires_at();


	//////////////////////////////////////////////////////////////////////////
	//      |      |  edgechain
	//      |______|
	//////////////////////////////////////////////////////////////////////////
	b2Vec2 vec[4];
	vec[0].Set(-100, 100);
	vec[1].Set(-100, 0);
	vec[2].Set(100, 0);
	vec[3].Set(100, 100);

	ptr_gameobject obj = boost::make_shared<staticobject>(m_world, m_makeindex,vec, 4);
	obj->initiate();
	m_gameobjectindexmap.insert(tgameobjectindexmap::value_type(obj->get_gameobjectindex(), obj));

	b2Vec2 vec2[2];
	vec2[0].Set(-10, 4);
	vec2[1].Set(10, 4);
	ptr_gameobject obj2 = boost::make_shared<staticobject>(m_world, m_makeindex, vec2, 2);
	obj2->initiate();
	m_gameobjectindexmap.insert(tgameobjectindexmap::value_type(obj2->get_gameobjectindex(), obj2));

	b2Vec2 mobposition(1.0f, 0.2f);
	ptr_gameobject obj3 = boost::make_shared<dumbmob>(m_world, m_makeindex, mobposition);
	obj3->initiate();
	m_gameobjectindexmap.insert(tgameobjectindexmap::value_type(obj3->get_gameobjectindex(), obj3));
	
	
}

void room::update()
{
	boost::posix_time::time_duration _duration = boost::asio::time_traits<boost::posix_time::ptime>::subtract(m_timer.expires_at(), m_pretime);
	float _seconds = _duration.total_milliseconds() / 1000.0f;
	
	m_pretime = m_timer.expires_at();
	m_timer.expires_at(m_timer.expires_at() + boost::posix_time::milliseconds(66));
	m_timer.async_wait(m_strand.wrap(boost::bind(&room::update, shared_from_this())));
	m_world->Step(_seconds, 10, 10);


	auto _indexcur = m_gameobjectindexmap.begin();
	while (_indexcur != m_gameobjectindexmap.end())
	{
		evmakedata evt;
		evtick _evttick(_seconds);

		_indexcur->second->process_event(_evttick); // tick
		_indexcur->second->process_event(evt); // make data
		_indexcur++;
		
	}

	auto _cur = m_usersessiongameobjectmap.begin();
	while (_cur != m_usersessiongameobjectmap.end())
	{
		auto _ptr = _cur->first.lock();
		if (_ptr != NULL)
		{
			ptr_packet_data _tcpdata = boost::make_shared<packet_data>();
			packet_encoder _tcpencode(_tcpdata);

			ptr_packet_data _udpdata = boost::make_shared<packet_data>();
			packet_encoder _udpencode(_udpdata);
		
			evmakepacketdata evti(&_tcpencode, &_udpencode);
			_cur->second->process_event(evti); // make data
			_tcpencode.makeheader();
			_udpencode.makeheader();
			if (_tcpdata->get_bodysize() != 0)
			{
				_ptr->do_writequeue(_tcpdata, packet_sendtype::tcp); // one tick info
			}
			if (_udpdata->get_bodysize() != 0)
			{
				_ptr->do_writequeue(_udpdata, packet_sendtype::udp); // all tick position data
			}

			_cur++;
		}
		else
		{
			m_gameobjectindexmap.erase(_cur->second->get_gameobjectindex());
			_cur = m_usersessiongameobjectmap.erase(_cur);
		}
	}

}

void room::dispatch_join(ptr_user_session arg_user, boost::function<void(ptr_room)> arg_roomprovider_handler, boost::function<void(ptr_room)> arg_user_handler)
{
	m_strand.dispatch([&, arg_user, arg_roomprovider_handler, arg_user_handler](){
		m_userset.insert(arg_user);

		ptr_gameobject obj = boost::make_shared<gameuser>(m_world,m_makeindex);

		obj->initiate();

		m_usersessiongameobjectmap.insert(tgameobjectmap::value_type( arg_user, obj));
		m_gameobjectindexmap.insert(tgameobjectindexmap::value_type(obj->get_gameobjectindex(),obj));

		ptr_packet_data _data = boost::make_shared<packet_data>();
		packet_encoder _encode(_data);

		obj->makepacket_info(&_encode);
		_encode.makeheader();
		if (_data->get_bodysize() != 0)
		{
			arg_user->do_writequeue(_data, packet_sendtype::tcp);
		}

		arg_roomprovider_handler(shared_from_this());
		arg_user_handler(shared_from_this());
	});
}

void room::dispatch_exit(ptr_user_session arg_user, boost::function<void(ptr_room)> arg_roomprovider_handler, boost::function<void()> arg_user_handler)
{
	m_strand.dispatch([&, arg_user, arg_roomprovider_handler, arg_user_handler](){
		m_userset.erase(arg_user);
		
		auto obj = m_usersessiongameobjectmap.find(arg_user);
		if (obj != m_usersessiongameobjectmap.end())
		{
			m_gameobjectindexmap.erase(obj->second->get_gameobjectindex());
		}
		
		m_usersessiongameobjectmap.erase(arg_user);		
		arg_roomprovider_handler(shared_from_this());
		arg_user_handler();
		
	});
}

ptr_gameobject room::get_gameobject(unsigned int arg_gameobjectindex)
{
	auto cura = m_gameobjectindexmap.find(arg_gameobjectindex);
	ptr_gameobject _gameobject;
	if (cura != m_gameobjectindexmap.end())
	{
		_gameobject = cura->second;
	}
	return _gameobject;
}



void room::process_gamemessage(ptr_user_session arg_user, ptr_proto_message arg_message, BYTE arg_type)
{

	m_strand.dispatch([&, arg_user, arg_message, arg_type](){
		if (arg_type == databody::move::descriptor()->index())
		{
			databody::move* _move = (databody::move*)arg_message.get();
			auto obj = m_usersessiongameobjectmap.find(arg_user);
			if (obj != m_usersessiongameobjectmap.end())
			{
				evmove evt(_move->direction());
				obj->second->process_event(evt);
			}
		}
		else if (arg_type == databody::jump::descriptor()->index())
		{
			auto obj = m_usersessiongameobjectmap.find(arg_user);
			if (obj != m_usersessiongameobjectmap.end())
			{
				evjump evt;
				obj->second->process_event(evt);
			}
		}
		else if (arg_type == databody::skill1::descriptor()->index())
		{
			auto obj = m_usersessiongameobjectmap.find(arg_user);
			if (obj != m_usersessiongameobjectmap.end())
			{
				evskill evt(skill1);
				obj->second->process_event(evt);

			}
		}
	});
}


bool room::check_tagstatus(const fixturetag& arg_taga , const fixturetag& arg_tagb, size_t arg_comparea ,bool& arg_result)
{
	
	if (arg_taga.getoption(arg_comparea) == true)
	{
		arg_result = 0;
		return true;
	}
	if (arg_tagb.getoption(arg_comparea) == true)
	{
		arg_result = 1; // swap
		return true;
	}

	arg_result = 0;
	return false;
}
bool room::check_tagstatus(const fixturetag& arg_tag, size_t arg_compare)
{
	if (arg_tag.getoption(arg_compare) == true)
	{
		return true;
	}

	return false;
}


void room::BeginContact(b2Contact* contact)
{
	fixturetag _taga((unsigned long)contact->GetFixtureA()->GetUserData());
	fixturetag _tagb((unsigned long)contact->GetFixtureB()->GetUserData());
	unsigned int _indexgameobjectA = (unsigned int)contact->GetFixtureA()->GetBody()->GetUserData();
	unsigned int _indexgameobjectB = (unsigned int)contact->GetFixtureB()->GetBody()->GetUserData();

	// check sight

	bool arg_result;
	if (check_tagstatus(_taga, _tagb, FixtureTag_SightRader, arg_result))
	{
		if (arg_result == true)
		{
			std::swap(_taga, _tagb);
			std::swap(_indexgameobjectA, _indexgameobjectB);
		}
		if (check_tagstatus(_tagb, FixtureTag_Body)
			|| check_tagstatus(_tagb, FixtureTag_Wire))
		{
			ptr_gameobject _obja = get_gameobject(_indexgameobjectA);
			ptr_gameobject _objb = get_gameobject(_indexgameobjectB);
			if (_obja != NULL && _objb != NULL)
			{
				evaddgameobject evt(_objb);
				_obja->process_event(evt);
			}
		}
	}

}

void room::EndContact(b2Contact* contact)
{
	fixturetag _taga((unsigned long)contact->GetFixtureA()->GetUserData());
	fixturetag _tagb((unsigned long)contact->GetFixtureB()->GetUserData());
	unsigned int _indexgameobjectA = (unsigned int)contact->GetFixtureA()->GetBody()->GetUserData();
	unsigned int _indexgameobjectB = (unsigned int)contact->GetFixtureB()->GetBody()->GetUserData();

	// check sight
	bool arg_result;
	if (check_tagstatus(_taga, _tagb, FixtureTag_SightRader, arg_result))
	{
		if (arg_result == true)
		{
			std::swap(_taga, _tagb);
			std::swap(_indexgameobjectA, _indexgameobjectB);
		}
		if (check_tagstatus(_tagb, FixtureTag_Body)
			|| check_tagstatus(_tagb, FixtureTag_Wire))
		{
			ptr_gameobject _obja = get_gameobject(_indexgameobjectA);
			ptr_gameobject _objb = get_gameobject(_indexgameobjectB);
			if (_obja != NULL && _objb != NULL)
			{
				evdeletegameobject evt(_objb);
				_obja->process_event(evt);
			}
		}
	}
}


void room::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	const b2Manifold* manifold = contact->GetManifold();

	if (manifold->pointCount == 0)
	{
		return;
	}

	fixturetag _taga((unsigned long)contact->GetFixtureA()->GetUserData());
	fixturetag _tagb((unsigned long)contact->GetFixtureB()->GetUserData());
	b2Vec2 _veca = contact->GetFixtureA()->GetBody()->GetPosition();
	b2Vec2 _vecb = contact->GetFixtureB()->GetBody()->GetPosition();


	bool arg_result;
	if (check_tagstatus(_taga, _tagb, FixtureTag_Wire, arg_result))
	{
		if (arg_result == true)
		{
			swap(_taga, _tagb);
			swap(_veca, _vecb);
		}
		if (check_tagstatus(_tagb, FixtureTag_Body))
		{
			b2PointState state1[b2_maxManifoldPoints], state2[b2_maxManifoldPoints];
			b2GetPointStates(state1, state2, oldManifold, manifold);

			b2WorldManifold worldManifold;
			contact->GetWorldManifold(&worldManifold);


			if (_vecb.y < worldManifold.points[0].y) // body 와 충돌포인트 높이 비교
			{
				contact->SetEnabled(false); // 충돌 처리를 off 할수 있다.
			}
		}
	}
}
