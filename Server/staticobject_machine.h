#pragma once
#include "def.h"
#include "event.h"
#include "packet_encoder.h"
#include "body.pb.h"

class staticobject_common;
class staticobject_machine;




class staticobject_machine : public sc::state_machine<staticobject_machine,staticobject_common>
{
	unsigned int m_gameobjectindex;
	ptr_b2world m_world;
	b2Body* m_body;
	databody::staticobject_info* m_info;
	gameobject* m_gameobject;


public:
	staticobject_machine(ptr_b2world arg_world, unsigned int arg_gameobjectindex, b2Vec2* arg_edgelist, int arg_edgecount, bool isguideline);
	~staticobject_machine();

	void makepacket_staticobject_info(b2Vec2* arg_edgelist, int arg_edgecount);

	databody::staticobject_info* get_staticobject_info();
};




class staticobject : public gameobject, public boost::enable_shared_from_this<staticobject>
{

	ptr_staticobject_machine m_machine;
	ptr_b2world m_world;
	b2Vec2* m_edgelist;
	int m_edgecount;
	bool isprocess = false;

public:
	staticobject(ptr_b2world arg_world, ptr_makeindex arg_makeindex, b2Vec2* arg_edgelist, int arg_edgecount, bool isguideline)
		:m_world(arg_world)
		, m_edgelist(arg_edgelist)
		, m_edgecount(arg_edgecount)
		, gameobject(arg_makeindex)
	{
		m_machine = boost::make_shared<staticobject_machine>(m_world, m_gameobjectindex, m_edgelist, m_edgecount, isguideline);
	}
	~staticobject()
	{	
		gameobject::~gameobject();
		delete m_edgelist;
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

	virtual void makepacket_info(packet_encoder* packet, bool control){
		auto _ptr = m_machine->get_staticobject_info();
		if (_ptr != NULL)
		{
			packet->addmessage(_ptr);
		}
	}	
protected:
private:
};

class staticobject_common : public sc::simple_state < staticobject_common, staticobject_machine >
{
public:
//	typedef mpl::list< sc::custom_reaction<evpacketinfo> > reactions;

	// make packet
//	sc::result react(const evpacketinfo &arg_evt);
};
