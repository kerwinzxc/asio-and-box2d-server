#include "stdafx.h"
#include "room.h"
#include "gameuser_machine.h"
#include "staticobject_machine.h"

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
	m_gameobjectset.insert(obj);
}

void room::update()
{
	boost::posix_time::time_duration _duration = boost::asio::time_traits<boost::posix_time::ptime>::subtract(m_timer.expires_at(), m_pretime);
	float _seconds = _duration.total_milliseconds() / 1000.0f;
	//cout << _seconds << endl;
	
	m_pretime = m_timer.expires_at();
	m_timer.expires_at(m_timer.expires_at() + boost::posix_time::milliseconds(66));
	m_timer.async_wait(m_strand.wrap(boost::bind(&room::update, shared_from_this())));
	m_world->Step(_seconds, 10, 10);


	auto _cur = m_gameobjectmap.begin();
	while (_cur != m_gameobjectmap.end())
	{
		evmakedata evt;
		evtick _evttick(_seconds);

		_cur->second->process_event(_evttick); // tick
		_cur->second->process_event(evt); // make data
		_cur++;
		
	}

	_cur = m_gameobjectmap.begin();
	while (_cur != m_gameobjectmap.end())
	{
		auto _ptr = _cur->first.lock();
		if (_ptr != NULL)
		{
			{
				ptr_packet_data _data = boost::make_shared<packet_data>();
				packet_encoder _encode(_data);
				evpacketinfolist evti(&_encode);
				_cur->second->process_event(evti); // make data
				_encode.makeheader();
				if (_data->get_bodysize() != 0)
				{
					_ptr->do_writequeue(_data, packet_sendtype::tcp); // one tick info
				}
			}

			{
				ptr_packet_data _data = boost::make_shared<packet_data>();
				packet_encoder _encode(_data);
				evpacketdatalist evtd(&_encode);
				_cur->second->process_event(evtd); // make data
				_encode.makeheader();
				if (_data->get_bodysize() != 0)
				{
					_ptr->do_writequeue(_data, packet_sendtype::udp); // all tick position data
				}
			}
			_cur++;
		}
		else
		{
			m_gameobjectset.erase(_cur->second);
			_cur = m_gameobjectmap.erase(_cur);
		}
	}

}

void room::dispatch_join(ptr_user_session arg_user, boost::function<void(ptr_room)> arg_roomprovider_handler, boost::function<void(ptr_room)> arg_user_handler)
{
	m_strand.dispatch([&, arg_user, arg_roomprovider_handler, arg_user_handler](){
		m_userset.insert(arg_user);

		ptr_gameobject obj = boost::make_shared<gameuser>(m_world,m_makeindex);

		obj->initiate();

		m_gameobjectmap.insert(tgameobjectmap::value_type( arg_user, obj));
		m_gameobjectset.insert(obj);

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
		
		auto obj = m_gameobjectmap.find(arg_user);
		if (obj != m_gameobjectmap.end())
		{
			m_gameobjectset.erase(obj->second);
		}
		
		m_gameobjectmap.erase(arg_user);		
		arg_roomprovider_handler(shared_from_this());
		arg_user_handler();
		
	});
}

void room::BeginContact(b2Contact* contact)
{
	gameobject* _gameobjectA = (gameobject*)contact->GetFixtureA()->GetBody()->GetUserData();
	int _fixtureA = (int) contact->GetFixtureA()->GetUserData();
	cout << "fixtureA" << _fixtureA << endl;
	gameobject* _gameobjectB = (gameobject*)contact->GetFixtureB()->GetBody()->GetUserData();
	int _fixtureB = (int)contact->GetFixtureB()->GetUserData();
	cout << "fixtureB" << _fixtureB << endl;


	if (_fixtureA == FixtureTag_GameuserBody)
	{
		
	}

	if (_fixtureA == FixtureTag_GameuserBodyNearRader1)
	{
		evaddgameobject evt(_gameobjectB);
		_gameobjectA->process_event(evt);
	}
	if (_fixtureB == FixtureTag_GameuserBodyNearRader1)
	{
		evaddgameobject evt(_gameobjectA);
		_gameobjectB->process_event(evt);
	}

	


	


	

	//update 
}

void room::EndContact(b2Contact* contact)
{

	gameobject* _gameobjectA = (gameobject*)contact->GetFixtureA()->GetBody()->GetUserData();
	int _fixtureA = (int)contact->GetFixtureA()->GetUserData();
	cout << "fixtureA" << _fixtureA << endl;
	gameobject* _gameobjectB = (gameobject*)contact->GetFixtureB()->GetBody()->GetUserData();
	int _fixtureB = (int)contact->GetFixtureB()->GetUserData();
	cout << "fixtureB" << _fixtureB << endl;


	if (_fixtureA == FixtureTag_GameuserBody)
	{

	}

	if (_fixtureA == FixtureTag_GameuserBodyNearRader1)
	{
		evdeletegameobject evt(_gameobjectB);
		_gameobjectA->process_event(evt);
	}
	if (_fixtureB == FixtureTag_GameuserBodyNearRader1)
	{
		evdeletegameobject evt(_gameobjectA);
		_gameobjectB->process_event(evt);
	}
	//update
}
