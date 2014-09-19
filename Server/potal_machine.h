#pragma once
#include "def.h"
#include "event.h"


class potal_common;
class potal_machine : public sc::state_machine<potal_machine, potal_common>
{
	unsigned int m_gameobjectindex;
	ptr_b2world m_world;
	b2Body* m_body;
	databody::staticobject_info* m_info;
	gameobject* m_gameobject;
	b2Vec2 m_sources;
	b2Vec2 m_destination;


public:
	potal_machine(ptr_b2world arg_world, int gameobjectindex, b2Vec2 arg_sources, b2Vec2 arg_destination);
	~potal_machine();

	b2Vec2 get_destination() const { return m_destination; }
};

class potal : public gameobject, public boost::enable_shared_from_this < potal >
{
	ptr_potal_machine m_machine;
	ptr_b2world m_world;
	bool isprocess = false;

public:
	potal(ptr_b2world arg_world, ptr_makeindex arg_makeindex, b2Vec2 arg_sources, b2Vec2 arg_destnation)
		: gameobject(arg_makeindex)
		, m_world(arg_world)

	{
		m_machine = boost::make_shared<potal_machine>(arg_world,m_gameobjectindex,arg_sources,arg_destnation);
	}
	~potal()
	{
		gameobject::~gameobject();
	}

	virtual void initiate()
	{
		m_machine->initiate();
	}

	virtual void process_event(const sc::event_base & evt)
	{
		if (isprocess == false)
		{
			isprocess = true;
			m_machine->process_event(evt);
		}
		else if (isprocess == true)
		{
			m_machine->post_event_impl(evt);
		}
		isprocess = false;
	}

	virtual void post_event(const sc::event_base & evt)
	{
		m_machine->post_event_impl(evt);
	}

	virtual void makepacket_info(packet_encoder* packet, bool control)
	{

	}
	virtual void makepacket_data(packet_encoder* packet)
	{
	}

};


class potal_common : public sc::simple_state < potal_common, potal_machine >
{
public:
	typedef mpl::list < sc::custom_reaction < evcontact >
		> reactions;

	sc::result react(const evcontact &arg_evt);

};