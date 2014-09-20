#include "stdafx.h"
#include "basemob_machine.h"


basemob_machine::basemob_machine(ptr_b2world arg_world, unsigned int arg_gameobjectindex, b2Vec2 arg_pos)
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
	fixturetag tag;
	tag.setoption(FixtureTag_Body, true);
	tag.setoption(FixtureTag_Mob, true);

	fd1.userData = (void*)tag.getvalue();
	fd1.density = 1.0f;
	fd1.friction = 1.0f;
	m_body->CreateFixture(&fd1);

	m_maxhp = 100.0f;
	m_curhp = m_maxhp;


	make_info();
}


basemob_machine::~basemob_machine()
{
}

void basemob_machine::make_info()
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

void basemob_machine::delete_data()
{
	if (m_data != NULL)
	{
		delete m_data;
		m_data = NULL;
	}
}

void basemob_machine::make_data()
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
	}
}

bool basemob_machine::checkdestory()
{
	if (m_curhp <= 0.0f)
	{
		return true;
	}
	return false;
}

void basemob_machine::onhit(float arg_dameage)
{
	m_body->ApplyForceToCenter(b2Vec2(0, arg_dameage * 2), true);
	m_curhp -= arg_dameage;
}

void basemob_machine::bodyleft()
{
	m_body->SetLinearVelocity(b2Vec2(-2.0f, 0.0f));
}

void basemob_machine::bodyright()
{
	m_body->SetLinearVelocity(b2Vec2(2.0f, 0.0f));
}

void basemob_machine::bodyjump()
{
	m_body->SetLinearVelocity(b2Vec2(0.0f, 10.0f));
}

void basemob_machine::bodynoting()
{
	m_body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
}

sc::result basemob_base::react(const evtick &arg_evt)
{
	if (context<basemob_machine>().checkdestory() == true)
	{
		(*arg_evt.m_destory) = true;
	}
	context<basemob_machine>().delete_data();
	return discard_event();
}

sc::result basemob_base::react(const evmakedata &arg_evt)
{
	context<basemob_machine>().make_data();
	return discard_event();
}

sc::result basemob_base::react(const evhit& arg_evt)
{
	context<basemob_machine>().onhit(arg_evt.m_dameage);
	return discard_event();
}

boost::random::mt19937 basemob_random::gen;
