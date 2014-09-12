#pragma once
#include "def.h"
#include "event.h"

class dumbmob_common;
class dumbmob_machine : public sc::state_machine<dumbmob_machine, dumbmob_common>
{
	unsigned int m_gameobjectindex;
	b2Body* m_body;
	ptr_b2world m_world;
	float m_maxhp;
	float m_curhp;

	databody::dumbmob_data* m_data;	
	databody::dumbmob_info* m_info;
	
	void make_info();
	
	
	
public:
	dumbmob_machine(ptr_b2world arg_world , int arg_gameobjectindex , b2Vec2 arg_pos);
	~dumbmob_machine();
	databody::dumbmob_info* get_info() const { return m_info; }
	databody::dumbmob_data* get_data() const { return m_data; }
	void delete_data();
	void make_data();
	
};




class dumbmob : public gameobject, public boost::enable_shared_from_this < dumbmob >
{

	ptr_dumbmob_machine m_machine;
	ptr_b2world m_world;

public:
	dumbmob(ptr_b2world arg_world, ptr_makeindex arg_makeindex, b2Vec2 arg_pos)
		:m_world(arg_world)
		, gameobject(arg_makeindex)
	{
		m_machine = boost::make_shared<dumbmob_machine>(arg_world, m_gameobjectindex, arg_pos);
	}

	virtual void initiate()
	{
		
		m_machine->initiate();
	}

	virtual void process_event(const sc::event_base & evt)
	{
		m_machine->process_event(evt);
	}

	virtual void makepacket_info(packet_encoder* arg_packet){
		arg_packet->addmessage(m_machine->get_info());
	}

	virtual void makepacket_data(packet_encoder* arg_packet){
		arg_packet->addmessage(m_machine->get_data());
	}
protected:
private:
};


class dumbmob_common : public sc::simple_state < dumbmob_common, dumbmob_machine >
{
public:
	typedef mpl::list < sc::custom_reaction < evtick >
		, sc::custom_reaction<evmakedata> > reactions;

	sc::result react(const evtick &arg_evt);
	sc::result react(const evmakedata &arg_evt);
};