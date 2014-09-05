#include "stdafx.h"
#include "staticobject_machine.h"
#include "util_makeindex.h"
#include "body.pb.h"
#include "packet_encoder.h"


staticobject_machine::staticobject_machine( ptr_b2world arg_world, unsigned int arg_gameobjectindex, b2Vec2* arg_edgelist, int arg_edgecount)
	: m_world(arg_world)
	, m_gameobjectindex(arg_gameobjectindex)
	, m_info(NULL)
{
	b2BodyDef bd;
	bd.position.Set(0.0f, 0.0f);
	bd.userData = (void*)m_gameobjectindex;
	m_body = arg_world->CreateBody(&bd);

	b2ChainShape chain;
	chain.CreateChain(arg_edgelist, arg_edgecount);

	b2FixtureDef fd;
	fd.shape = &chain;
	fd.userData = (void*)(int)FixtureTag_MapObject;

	m_body->CreateFixture(&fd);

	makepacket_staticobject_info(arg_edgelist, arg_edgecount);


}


staticobject_machine::~staticobject_machine()
{
	m_world->DestroyBody(m_body);
	delete m_info;
}

void staticobject_machine::makepacket_staticobject_info(b2Vec2* arg_edgelist, int arg_edgecount)
{
	m_info = new databody::staticobject_info();
	for (int i = 0; i < arg_edgecount; i++)
	{
		m_info->add_x(arg_edgelist[i].x);
		m_info->add_y(arg_edgelist[i].y);
	}

	m_info->set_gameobject_index(m_gameobjectindex);

	m_info->set_angle(0.0f);
	m_info->set_posx(0.0f);
	m_info->set_posy(0.0f);

}

databody::staticobject_info* staticobject_machine::get_staticobject_info()
{
	return m_info;
}
