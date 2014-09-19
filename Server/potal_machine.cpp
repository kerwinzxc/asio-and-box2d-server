#include "stdafx.h"
#include "potal_machine.h"


potal_machine::potal_machine(ptr_b2world arg_world, int arg_gameobjectindex, b2Vec2 arg_sources, b2Vec2 arg_destnation)
	:m_world(arg_world)
	, m_gameobjectindex(arg_gameobjectindex)
	, m_sources(arg_sources)
	, m_destination(arg_destnation)
{
	b2BodyDef def;
	def.position = m_sources;
	def.userData = (void*)m_gameobjectindex;
	m_body = m_world->CreateBody(&def);

	b2PolygonShape shape;
	shape.SetAsBox(2.0f, 2.0f);
	b2FixtureDef fd;
	fd.shape = &shape;
	fd.isSensor = true;
	fixturetag tag;
	tag.setoption(FixtureTag_PotalRader,true);
	fd.userData = (void*)tag.getvalue();
	m_body->CreateFixture(&fd);
}


potal_machine::~potal_machine()
{
}

sc::result potal_common::react(const evcontact &arg_evt)
{
	evforcemove evt(context<potal_machine>().get_destination());
	arg_evt.m_obj->process_event(evt);
	return discard_event();
}
