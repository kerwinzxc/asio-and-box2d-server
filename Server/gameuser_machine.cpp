#include "stdafx.h"
#include "gameuser_machine.h"
#include "util_makeindex.h"
#include "body.pb.h"
#include "packet_encoder.h"


gameuser_machine::gameuser_machine(gameobject* arg_gameobject, ptr_b2world arg_world, ptr_makeindex arg_makeindex)
	:m_world(arg_world)
	,m_makeindex(arg_makeindex)
	,m_info(NULL)
	,m_data(NULL)
	,m_gameobject(arg_gameobject)
{
	m_skillcooltime[0] = 1.0f;
	m_skillcooltime[1] = 1.0f;
	m_revivedtime = 1.9f;
	m_maxplayerhp = 100;
	m_curplayerhp = m_maxplayerhp;

	m_gameobjectindex = m_makeindex->useindex(gameobjectindextag_gameuser);

	b2Vec2 x(-7.0f, 0.75f);
	b2Vec2 y(-7.0f, 20.0f);
	b2Vec2 deltaX(0.5625f, 1.25f);
	b2Vec2 deltaY(1.125f, 0.0f);



	// body
	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.position = y;
	bd.userData = (void*)arg_gameobject;
	m_body = m_world->CreateBody(&bd);
	
	//box
	b2PolygonShape shape;
	shape.SetAsBox(0.5f,1.0f);

	b2FixtureDef fd1;
	fd1.shape = &shape;
	fd1.userData = (void*)(int)FixtureTag_GameuserBody;
	fd1.density = 1.0f;
	m_body->CreateFixture(&fd1);
//

// circle sensor 일정 원안에 들어갈경우 개체의 정보를 보낸다.
	b2CircleShape circleshape;
	circleshape.m_radius = 30.0f;
	circleshape.m_p.Set(0.0f, 0.0f);

	b2FixtureDef fd;
	fd.shape = &circleshape;
	fd.isSensor = true;
	fd.userData = (void*)(int)FixtureTag_GameuserBodyNearRader1;
	m_body->CreateFixture(&fd);

	/*
	b2EdgeShape edge;
	edge.Set(b2Vec2(0.0f, 0.0f), b2Vec2(0.0f, 1.0f));
	b2FixtureDef edgefd;
	edgefd.shape = &edge;
	edgefd.isSensor = true;
	b2Fixture* fix = m_body->CreateFixture(&edgefd);
	*/
	//m_world
	
//	m_body->SetUserData()
//
	makepacket_gameuser_info();
}


gameuser_machine::~gameuser_machine()
{

	m_world->DestroyBody(m_body);
	m_makeindex->reuseindex(m_gameobjectindex);

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
		m_data->set_state(1);
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

gameuser_live::gameuser_live()
{

}

gameuser_live::~gameuser_live()
{

}

gameuser_idle::gameuser_idle()
{

}

gameuser_idle::~gameuser_idle()
{

}

sc::result gameuser_idle::react(const evtick &arg_evt)
{
	return forward_event();
}

sc::result gameuser_idle::react(const evmove &arg_evt)
{
	post_event(arg_evt);
	return transit<gameuser_move>();
}

sc::result gameuser_idle::react(const evskill<skilltype::skill1> & arg_evt)
{
	post_event(arg_evt);
	return transit<gameuser_skill<skilltype::skill1>>();
}

sc::result gameuser_idle::react(const evskill<skilltype::skill2> & arg_evt)
{
	post_event(arg_evt);
	return transit<gameuser_skill<skilltype::skill2>>();
}

gameuser_move::gameuser_move()
{

}

gameuser_move::~gameuser_move()
{

}

sc::result gameuser_move::react(const evtick &arg_evt)
{
	return forward_event();
}

sc::result gameuser_move::react(const evmove &arg_evt)
{
	m_evmove = arg_evt;
	return discard_event();
}

sc::result gameuser_move::react(const evskill<skilltype::skill1> & arg_evt)
{	
	post_event(arg_evt);
	post_event(m_evmove); // 다시 무브로 옴..
	return transit<gameuser_skill<skilltype::skill1>>();
}

sc::result gameuser_move::react(const evskill<skilltype::skill2> & arg_evt)
{
	post_event(arg_evt);
	post_event(m_evmove);
	return transit<gameuser_skill<skilltype::skill2>>();
}

template<skilltype _skilltype>
gameuser_skill<_skilltype>::gameuser_skill()
{
	skill_interface* p = NULL;
	if (_skilltype == skilltype::skill1)
	{
		m_skillinterface = new skill1_def();
	}
	if (_skilltype == skilltype::skill2)
	{
		m_skillinterface = new skill2_def();
	}
	
}

template<skilltype _skilltype>
gameuser_skill<_skilltype>::~gameuser_skill()
{
	delete m_skillinterface;
}

template<skilltype _skilltype>
sc::result gameuser_skill<_skilltype>::react(const evtick &arg_evt)
{
	m_cooltime += arg_evt.m_tick;

	m_skillinterface->tick(arg_evt);


	if (m_cooltime > context<gameuser_machine>().m_skillcooltime[_skilltype])
	{
		return transit< gameuser_idle >();
	}
	
	return forward_event();
}


gameuser_dead::gameuser_dead()
	:m_cooltime(0.0f)
{

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

sc::result gameuser_common::react(const evpacketinfolist &arg_evt)
{
	auto& _gameuser_machine = context<gameuser_machine>();
	auto curitr = _gameuser_machine.m_infolist.begin();
	while (curitr != _gameuser_machine.m_infolist.end())
	{
		auto _sharedptr = (*curitr);
		if (_sharedptr != NULL)
		{
			_sharedptr->makepacket_info(arg_evt.m_packet);			
		}
		curitr++;
	}

	_gameuser_machine.m_infolist.clear();

	return discard_event();
}

sc::result gameuser_common::react(const evpacketdatalist &arg_evt)
{
	auto& _gameuser_machine = context<gameuser_machine>();

	auto curitr = context<gameuser_machine>().m_datalist.begin();
	while (curitr != context<gameuser_machine>().m_datalist.end())
	{
		auto _sharedptr = (*curitr);
		if (_sharedptr != NULL)
		{
			_sharedptr->makepacket_data(arg_evt.m_packet);
			curitr++;
		}
	}
	arg_evt.m_packet->addmessage(_gameuser_machine.get_gameuser_data());
	return discard_event();
}



gameuser_common::gameuser_common()
{
}

gameuser_common::~gameuser_common()
{

}
