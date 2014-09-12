#include "stdafx.h"
#include "dumbmob_machine.h"


dumbmob_machine::dumbmob_machine(ptr_b2world arg_world, int arg_gameobjectindex, b2Vec2 arg_pos)
	:m_world(arg_world)
	, m_gameobjectindex(arg_gameobjectindex)
{
	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.position = arg_pos;
	bd.userData = (void*)m_gameobjectindex;
	bd.fixedRotation = true;
	m_body = m_world->CreateBody(&bd);

	//box
	b2PolygonShape shape;
	shape.SetAsBox(0.5f, 0.5f);

	b2FixtureDef fd1;
	fd1.shape = &shape;
	fd1.userData = (void*)(int)FixtureTag_MobBody;
	fd1.density = 1.0f;
	m_body->CreateFixture(&fd1);


	make_info();
}


dumbmob_machine::~dumbmob_machine()
{
}

void dumbmob_machine::make_info()
{
	if (m_info == NULL)
	{
		m_info = new databody::dumbmob_info();
		m_info->set_gameobject_index(m_gameobjectindex);
		m_info->set_mass(m_body->GetMass());
		m_info->set_max_hp(m_maxhp);
		m_info->set_type(1);
	}
}

void dumbmob_machine::make_data()
{
	if (m_data == NULL)
	{
		m_data = new databody::dumbmob_data();
		m_data->set_angle(m_body->GetAngle());
		m_data->set_cur_hp(m_curhp);
		m_data->set_gameobject_index(m_gameobjectindex);
		b2Vec2 pos = m_body->GetPosition();
		m_data->set_posx(pos.x);
		m_data->set_posy(pos.y);
		b2Vec2 vel = m_body->GetLinearVelocity();
		m_data->set_velx(vel.x);
		m_data->set_vely(vel.y);

		cout << pos.x << " " << pos.y << endl;
	}
}
void dumbmob_machine::delete_data()
{
	if (m_data != NULL)
	{
		delete m_data;
		m_data = NULL;
	}
}

sc::result dumbmob_common::react(const evtick &arg_evt)
{
	context<dumbmob_machine>().delete_data();
	return discard_event();
}

sc::result dumbmob_common::react(const evmakedata &arg_evt)
{
	context<dumbmob_machine>().make_data();
	return discard_event();
}
