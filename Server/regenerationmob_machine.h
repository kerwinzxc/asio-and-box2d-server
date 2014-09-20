#pragma once

#include "def.h"
#include "event.h"

class regenerationmob_common;

class regenerationmob_machine : public sc::state_machine<regenerationmob_machine, regenerationmob_common>
{
	ptr_b2world m_world;
	b2Body* m_body;
	unsigned int m_gameobjectindex;
	float m_left;
	float m_right;
	float m_top;
	float m_bottom;
	float m_regenerationtime;
	unsigned int m_maxmobcount;
	weakptr_room m_room;

	boost::mt19937 rng;

	set< weakptr_gameobject > m_setgameobject;
public:
	regenerationmob_machine(ptr_b2world arg_world, weakptr_room arg_room, unsigned int arg_gameobjectindex, float arg_left, float arg_right, float arg_top, float arg_bottom, float arg_regenerationtime, unsigned int arg_maxmobcount);
	~regenerationmob_machine();

	bool CreateRandomMob(float arg_tick);
};


class regenerationmob : public gameobject, public boost::enable_shared_from_this < regenerationmob >
{
	ptr_regenerationmob_machine m_machine;
	ptr_b2world m_world;
	bool isprocess = false;

public:
	regenerationmob(ptr_b2world arg_world,weakptr_room arg_room, ptr_makeindex arg_makeindex, float arg_left, float arg_right, float arg_top, float arg_bottom , float arg_regenerationtime , unsigned int arg_maxmobcount )
		: gameobject(arg_makeindex)
		, m_world(arg_world)

	{
		m_machine = boost::make_shared<regenerationmob_machine>(arg_world, arg_room, m_gameobjectindex, arg_left, arg_right, arg_top, arg_bottom,arg_regenerationtime,arg_maxmobcount);
	}
	~regenerationmob()
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


class regenerationmob_common : public sc::simple_state < regenerationmob_common, regenerationmob_machine >
{
	float m_addtick;
public:
	typedef mpl::list < sc::custom_reaction < evtick >
	> reactions;

	sc::result react(const evtick &arg_evt);

};