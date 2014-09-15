#include "stdafx.h"
#include "gameuser_machine.h"
#include "util_makeindex.h"
#include "body.pb.h"
#include "packet_encoder.h"



gameuser_machine::gameuser_machine( ptr_b2world arg_world, unsigned arg_gameobjectindex)
	:m_world(arg_world)
	,m_gameobjectindex(arg_gameobjectindex)
	,m_info(NULL)
	,m_data(NULL)
{
	m_skillcooltime[0] = 1.0f;
	m_skillcooltime[1] = 1.0f;
	m_revivedtime = 1.9f;
	m_maxplayerhp = 100;
	m_curplayerhp = m_maxplayerhp;

	b2Vec2 x(-7.0f, 0.75f);
	b2Vec2 y(-7.0f, 20.0f);
	b2Vec2 deltaX(0.5625f, 1.25f);
	b2Vec2 deltaY(1.125f, 0.0f);



	// body
	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.position = y;
	bd.userData = (void*)m_gameobjectindex;
	bd.fixedRotation = true;
	m_body = m_world->CreateBody(&bd);
	
	//box
	b2PolygonShape shape;
	shape.SetAsBox(0.5f,1.0f);

	b2FixtureDef fd1;
	fd1.shape = &shape;

	fixturetag tagbody;
	tagbody.setoption(FixtureTag_Body,true);
	tagbody.setoption(FixtureTag_Gameuser,true);

	fd1.userData = (void*)tagbody.getvalue();
	fd1.density = 1.0f;
	m_body->CreateFixture(&fd1);
	
//

// circle sensor 일정 원안에 들어갈경우 개체의 정보를 보낸다.
	b2CircleShape circleshape;
	circleshape.m_radius = 100.0f;
	circleshape.m_p.Set(0.0f, 0.0f);

	b2FixtureDef fd;
	fd.shape = &circleshape;
	fd.isSensor = true;

	tagbody.reset();
	tagbody.setoption(FixtureTag_SightRader, true);
	tagbody.setoption(FixtureTag_Gameuser, true);
	fd.userData = (void*)tagbody.getvalue();
	m_body->CreateFixture(&fd);

	makepacket_gameuser_info();
	m_StateType = 0;

}


gameuser_machine::~gameuser_machine()
{

	m_world->DestroyBody(m_body);
	delete m_info;
	deletepacket_gameuser_data();

}

void gameuser_machine::makepacket_gameuser_info()
{
	if (m_info == NULL)
	{
		m_info = new databody::gameuser_info();

		m_info->set_gameobject_index(m_gameobjectindex);
		m_info->set_max_hp(m_maxplayerhp);
		m_info->set_mass(m_body->GetMass());
	}

}

void gameuser_machine::deletepacket_gameuser_data()
{
	if (m_data != NULL)
	{
		delete m_data;
		m_data = NULL;
	}
}

void gameuser_machine::makepacket_gameuser_data()
{
	if (m_data == NULL)
	{
		m_data = new databody::gameuser_data();
		m_data->set_angle(m_body->GetAngle());
		m_data->set_cur_hp(m_curplayerhp);
		m_data->set_gameobject_index(m_gameobjectindex);
		const b2Vec2& vec = m_body->GetPosition();
		m_data->set_posx(vec.x);
		m_data->set_posy(vec.y);
		auto& lv = m_body->GetLinearVelocity(); // 선속도
		m_data->set_velx(lv.x);
		m_data->set_vely(lv.y);
		
		m_body->GetAngularVelocity(); // 각속도
		m_data->set_state(m_StateType);
		m_data->set_swordangle(m_swordangle);
	}
}

databody::gameuser_info* gameuser_machine::get_gameuser_info() const
{
	return m_info;
}

databody::gameuser_data* gameuser_machine::get_gameuser_data() const
{
	return m_data;
}

void gameuser_machine::RayCast(raycastcallback* callback, const b2Vec2& point, const float& angle) const
{
	float32 L = 2.0f;
	b2Vec2 point1 = m_body->GetPosition();
	point1 += point;
	b2Vec2 d(L * cosf(angle * (b2_pi / 180)), L * sinf(angle * (b2_pi / 180)));
	b2Vec2 point2 = point1 + d;
	callback->m_ignorebody;
	m_world->RayCast(callback, point1, point2);
}

gameuser_live::gameuser_live()
{

}

gameuser_live::~gameuser_live()
{

}

gameuser_idle::gameuser_idle()
{
	//context<gameuser_machine>().SetStateType(1);
}

gameuser_idle::~gameuser_idle()
{

}

sc::result gameuser_idle::react(const evtick &arg_evt)
{
	//return transit<gameuser_skill1>();
	return forward_event();
}

sc::result gameuser_idle::react(const evmove &arg_evt)
{
	post_event(arg_evt);
	return transit<gameuser_move>();
}

sc::result gameuser_idle::react(const evjump &arg_evt)
{
	post_event(arg_evt);
	return transit<gameuser_move>();
}

sc::result gameuser_idle::react(const evskill & arg_evt)
{
	if (arg_evt.m_skilltype == skill1)
		return transit<gameuser_skill1>();
	else if (arg_evt.m_skilltype == skill2)
		return transit<gameuser_skill2>();
	else
		return discard_event();
}


gameuser_move::gameuser_move()
	:m_evmove(databody::movedirectiontype::_none)
	, m_jumped(false)
{
	//context<gameuser_machine>().SetStateType(2);
}

gameuser_move::~gameuser_move()
{

}

sc::result gameuser_move::react(const evtick &arg_evt)
{
	auto lv = context<gameuser_machine>().m_body->GetLinearVelocity(); // 점프했을때 y축 속도는 남김.
	if (m_evmove.m_type == databody::movedirectiontype::_left)
	{
		auto a = context<gameuser_machine>().m_body->GetLinearVelocity(); // 점프했을때 y축 속도는 남김.

		if (lv.y == 0.0f)
		{
			context<gameuser_machine>().m_body->SetLinearVelocity(b2Vec2(-10.0f, lv.y));
		}
		else
		{
			context<gameuser_machine>().m_body->SetLinearVelocity(b2Vec2(-5.0f, lv.y));
		}
		
	}
	else if (m_evmove.m_type == databody::movedirectiontype::_right)
	{
		auto a = context<gameuser_machine>().m_body->GetLinearVelocity(); // 점프했을때 y축 속도는 남김.
		if (lv.y == 0.0f)
		{
			context<gameuser_machine>().m_body->SetLinearVelocity(b2Vec2(10.0f, lv.y));
		}
		else
		{
			context<gameuser_machine>().m_body->SetLinearVelocity(b2Vec2(5.0f, lv.y));
		}
	}
	else if (m_evmove.m_type == databody::movedirectiontype::_end
		|| m_evmove.m_type == databody::movedirectiontype::_none)
	{
		
		context<gameuser_machine>().m_body->SetLinearVelocity(b2Vec2(0.0f, lv.y));		
	}
	lv = context<gameuser_machine>().m_body->GetLinearVelocity(); // 점프했을때 y축 속도는 남김.
	if (lv.x == 0.0f && lv.y == 0.0f)
	{
		return transit<gameuser_idle>();
	}
	
	return forward_event();
}

sc::result gameuser_move::react(const evmove &arg_evt)
{
	m_evmove = arg_evt;
	return discard_event();
}

sc::result gameuser_move::react(const evjump &arg_evt)
{	
	if (m_jumped == false)
	{
		m_jumped = true;
		auto a = context<gameuser_machine>().m_body->GetLinearVelocity(); // 점프했을때 y축 속도는 남김.
		context<gameuser_machine>().m_body->SetLinearVelocity(b2Vec2(a.x, 10.0f));
	}
	return discard_event();
}

sc::result gameuser_move::react(const evskill & arg_evt)
{
	//post_event(arg_evt);
	//post_event(m_evmove); // 다시 무브로 옴..
	//return transit<gameuser_skill<skilltype::skill1>>();

	return discard_event();
}


gameuser_skill1::gameuser_skill1()
{
	m_angle = 100.0f;
	init = true;
	loop = false;
	end = false;
	//context<gameuser_machine>().SetStateType(3);
}

gameuser_skill1::~gameuser_skill1()
{

}

sc::result gameuser_skill1::react(const evtick &arg_evt)
{


	if (m_angle > 360.0f)
	{
		m_angle -= 360.0f;
	}
	if (init == true)
	{

		loop = true;
		init = false;
		return discard_event();
	}
	if (loop == true)
	{
		m_angle += arg_evt.m_tick * 250;

		if (m_angle > 100.0f && m_angle < 230.0f)
		{


			RayCastClosestCallback a;
			b2Vec2 vec(0.0f, 0.0f);

			context<gameuser_machine>().m_swordangle = m_angle * (b2_pi / 180);
			context<gameuser_machine>().RayCast(&a, vec, m_angle);
			if (a.m_hit == true)
				cout << "hit  angle" << m_angle << endl;

		}
		else
		{
			context<gameuser_machine>().m_swordangle = 100 * (b2_pi / 180);
		}

		if (m_angle > 240)
		{
			loop = false;
			end = true;
			
		}
		return discard_event();
	}

	if (end = true)
	{
		return transit<gameuser_idle>();
	}


	
	
	
	
	/*
	m_cooltime += arg_evt.m_tick;

	m_skillinterface->tick(arg_evt);


	if (m_cooltime > context<gameuser_machine>().m_skillcooltime[_skilltype])
	{
		return transit< gameuser_idle >();
	}
	*/
	
	return forward_event();
}


gameuser_skill2::gameuser_skill2()
{
	//context<gameuser_machine>().SetStateType(4);
}

gameuser_skill2::~gameuser_skill2()
{

}

sc::result gameuser_skill2::react(const evtick &arg_evt)
{
	return discard_event();
}



gameuser_dead::gameuser_dead()
	:m_cooltime(0.0f)
{
	//context<gameuser_machine>().SetStateType(5);
}

gameuser_dead::~gameuser_dead()
{

}

sc::result gameuser_dead::react(const evtick &arg_evt)
{
	m_cooltime += arg_evt.m_tick;
	if (m_cooltime > context<gameuser_machine>().m_revivedtime)
	{
		return transit< gameuser_live >();
	}
	
	return forward_event();
	
}

gameuser_condition::gameuser_condition()
{

}

gameuser_condition::~gameuser_condition()
{

}

sc::result gameuser_condition::react(const evtick &arg_evt)
{
	context<gameuser_machine>().m_curplayerhp -= arg_evt.m_tick;

	if (context<gameuser_machine>().m_curplayerhp < 0)
	{
		return transit<gameuser_dead>();
	}
	
	return forward_event();
}

sc::result gameuser_common::react(const evmakedata &arg_evt)
{
	context<gameuser_machine>().makepacket_gameuser_data();
	return discard_event();
}

sc::result gameuser_common::react(const evtick &arg_evt)
{
	context<gameuser_machine>().deletepacket_gameuser_data(); // 한틱에 여러번 불릴수 있다.
	return forward_event();
}

sc::result gameuser_common::react(const evaddgameobject &arg_evt)
{
	context<gameuser_machine>().addgameobject(arg_evt.m_gameobject);
	return discard_event();
}
sc::result gameuser_common::react(const evdeletegameobject &arg_evt)
{
	context<gameuser_machine>().deletegameobject(arg_evt.m_gameobject);
	return discard_event();
}

sc::result gameuser_common::react(const evmakepacketdata &arg_evt)
{
	{

	
		auto& _gameuser_machine = context<gameuser_machine>();
		auto curitr = _gameuser_machine.m_infolist.begin();
		while (curitr != _gameuser_machine.m_infolist.end())
		{		
			auto _sharedptr = curitr->first.lock();
			if (_sharedptr != NULL)
			{
				_sharedptr->makepacket_info(arg_evt.m_tcppacket);
				curitr++;
			}
			else
			{
				curitr == _gameuser_machine.m_infolist.erase(curitr);
			}
		}

		_gameuser_machine.m_infolist.clear();
	}

	{
		auto& _gameuser_machine = context<gameuser_machine>();

		auto curitr = context<gameuser_machine>().m_datalist.begin();
		while (curitr != context<gameuser_machine>().m_datalist.end())
		{
			auto _sharedptr = curitr->first.lock();
			if (_sharedptr != NULL)
			{
				_sharedptr->makepacket_data(arg_evt.m_udppacket);
				curitr++;
			}
			else
			{
				int index = curitr->second;
				databody::leave_object obj;
				obj.set_gameobject_index(index);
				arg_evt.m_tcppacket->addmessage(&obj);

				curitr == _gameuser_machine.m_datalist.erase(curitr);
			}
		}
		arg_evt.m_udppacket->addmessage(_gameuser_machine.get_gameuser_data());
	}
	return discard_event();
}

gameuser_common::gameuser_common()
{
}

gameuser_common::~gameuser_common()
{

}
