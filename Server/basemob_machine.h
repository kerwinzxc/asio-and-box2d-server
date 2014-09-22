#pragma once
#include "def.h"
#include "event.h"

class basemob_base;
class basemob_random;
class basemob_jump;
class basemob_left;
class basemob_right;
class basemob_nothing;

class basemob_machine : public sc::state_machine<basemob_machine, basemob_base>
{
	unsigned int m_gameobjectindex;
	ptr_b2world m_world;
	b2Body* m_body;

	float m_maxhp;
	float m_curhp;

	databody::dumbmob_data* m_data;
	databody::dumbmob_info* m_info;

	databody::movedirectiontype m_dir;
	
	
	void make_info();
	

public:
	basemob_machine(ptr_b2world arg_world, unsigned int arg_gameobjectindex, b2Vec2 arg_pos);
	~basemob_machine();

	void delete_data();
	void make_data();

	databody::dumbmob_info* get_info() const { return m_info; }
	databody::dumbmob_data* get_data() const { return m_data; }

	bool checkdestory();
	void onhit(float arg_dameage, databody::movedirectiontype arg_dir);

	void bodyleft();
	void bodyright();
	void bodyjump();
	void bodynoting();
	bool iszerovelocity();
	bool checkguideline(int& arg_direction);

	databody::movedirectiontype get_dir() const { return m_dir; }
};

class basemob : public gameobject, public boost::enable_shared_from_this < basemob >
{
	ptr_basemob_machine m_machine;
	ptr_b2world m_world;

	bool isprocess = false;
public:
	basemob(ptr_b2world arg_world, ptr_makeindex arg_makeindex, b2Vec2 arg_pos)
		:m_world(arg_world)
		, gameobject(arg_makeindex)
	{
		m_machine = boost::make_shared<basemob_machine>(m_world , m_gameobjectindex , arg_pos);
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

	virtual void makepacket_info(packet_encoder* arg_packet, bool control){
		arg_packet->addmessage(m_machine->get_info());
	}

	virtual void makepacket_data(packet_encoder* arg_packet){
		arg_packet->addmessage(m_machine->get_data());
	}
};

// 기본 몹 정의 상태 정의 
//////////////////////////////////////////////////////////////////////////
// simple_state
// base
//
// jump // leftmove // rightmove // notthing //  state end
// ->random
//////////////////////////////////////////////////////////////////////////

class basemob_base : public sc::simple_state < basemob_base, basemob_machine, basemob_nothing >
{
public:
	typedef mpl::list < sc::custom_reaction < evtick >
		, sc::custom_reaction<evmakedata>
		, sc::custom_reaction < evhit >
		, sc::custom_reaction <evcontact> > reactions;

	sc::result react(const evtick &arg_evt);
	sc::result react(const evmakedata &arg_evt);
	sc::result react(const evhit& arg_evt);
	sc::result react(const evcontact& arg_evt);
};


class basemob_random : public sc::simple_state < basemob_random, basemob_base >
{
public :
	typedef mpl::list < sc::custom_reaction < evtick >> reactions;
	static boost::random::mt19937 gen;


	sc::result react(const evtick &arg_evt)
	{
		double probabilities[] = {
			0.1, 0.4, 0.4, 0.1
		};
		boost::random::discrete_distribution<> dist(probabilities);
		switch (dist(gen))
		{
		case 0:
			return transit<basemob_nothing>();
			break;
		case 1:
			return transit<basemob_left>();
			break;
		case 2:
			return transit<basemob_right>();
			break;
		case 3:
			return transit<basemob_jump>();
			break;
		}

		return forward_event();
	}
};

class basemob_nothing : public sc::simple_state < basemob_nothing, basemob_base >
{
public:
	typedef mpl::list < sc::custom_reaction < evtick >> reactions;
	float m_remaintime;
	basemob_nothing()
	{
		m_remaintime = 2.0f;
	}
	sc::result react(const evtick &arg_evt)
	{
		context<basemob_machine>().bodynoting();
		if (m_remaintime < 0.0f)
		{
			return transit<basemob_random>();
		}
		m_remaintime -= arg_evt.m_tick;
		return forward_event();
	}
	
};

class basemob_left : public sc::simple_state < basemob_left, basemob_base >
{
public:
	typedef mpl::list < sc::custom_reaction < evtick >> reactions;
	float m_remaintime;
	basemob_left()
	{
		m_remaintime = 4.0f;
	}
	sc::result react(const evtick &arg_evt)
	{
		// moveleft
		

		if (m_remaintime < 0.0f)
		{
			if (context<basemob_machine>().iszerovelocity() == true)
			{
				return transit<basemob_random>();
			}
		}
		else
		{
			context<basemob_machine>().bodyleft();
		}
		m_remaintime -= arg_evt.m_tick;
		return forward_event();
	}
};
class basemob_right : public sc::simple_state < basemob_right, basemob_base >
{
public:
	typedef mpl::list < sc::custom_reaction < evtick >> reactions;
	float m_remaintime;
	basemob_right()
	{
		m_remaintime = 4.0f;
	}
	sc::result react(const evtick &arg_evt)
	{
		// moveright
		

		if (m_remaintime < 0.0f)
		{
			if (context<basemob_machine>().iszerovelocity() == true)
			{
				return transit<basemob_random>();
			}
			
		}
		else
		{
			context<basemob_machine>().bodyright();
		}
		m_remaintime -= arg_evt.m_tick;
		return forward_event();
	}
};
class basemob_jump : public sc::simple_state < basemob_jump, basemob_base >
{
public:
	typedef mpl::list < sc::custom_reaction < evtick >> reactions;
	float m_remaintime;
	bool isjumped = false;
	basemob_jump()
	{
		m_remaintime = 2.0f;
	}
	sc::result react(const evtick &arg_evt)
	{
		// movejump
		if (isjumped == false)
		{
			context<basemob_machine>().bodyjump();
			isjumped = true;
		}	

		if (context<basemob_machine>().iszerovelocity() == true)
		{
			return transit<basemob_random>();
		}
		m_remaintime -= arg_evt.m_tick;
		return forward_event();
	}
};