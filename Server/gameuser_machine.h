#pragma once
#include "def.h"
#include "gameuser_skilldef.h"
#include "event.h"
#include "body.pb.h"
#include "packet_encoder.h"


struct istatetype 
{
	virtual int getstatetype() const = 0;
};


class gameuser_machine : public sc::state_machine < gameuser_machine, gameuser_common >
{

public:
	float m_skillcooltime[2];
	float m_revivedtime;
	float m_movespeed;
	float m_curplayerhp;
	float m_maxplayerhp;

	unsigned int m_gameobjectindex;

	ptr_makeindex m_makeindex;
	gameobject* m_gameobject;


	b2Body* m_body;
	b2Body* m_sword;

	ptr_b2world m_world;

	databody::gameuser_info* m_info;
	databody::gameuser_data* m_data;

	databody::gameuser_info* get_gameuser_info() const;
	databody::gameuser_data* get_gameuser_data() const;

	set<gameobject*> m_infolist;
	set<gameobject*> m_datalist;


	gameuser_machine(gameobject* arg_gameobject, ptr_b2world arg_world, ptr_makeindex arg_makeindex);
	~gameuser_machine();

	void makepacket_gameuser_info();

	void makepacket_gameuser_data();
	void deletepacket_gameuser_data();

	void addgameobject(gameobject* arg_gameobject)
	{
		m_infolist.insert(arg_gameobject);
		m_datalist.insert(arg_gameobject);
	}
	void deletegameobject(gameobject* arg_gameobject)
	{
		m_infolist.erase(arg_gameobject);
		m_datalist.erase(arg_gameobject);
	}
	
};

class gameuser : public gameobject, public boost::enable_shared_from_this<gameuser>
{
public :
	ptr_gameuser_machine m_machine;
	ptr_b2world m_world;
	ptr_makeindex m_makeindex;
	gameuser(ptr_b2world arg_world, ptr_makeindex arg_makeindex)
		: m_world(arg_world)
		, m_makeindex(arg_makeindex)
	{
		
	}

	virtual void initiate()
	{
		m_machine = boost::make_shared<gameuser_machine>(this, m_world, m_makeindex);
		m_machine->initiate();
	}

	virtual void process_event(const sc::event_base & evt)
	{
		m_machine->process_event(evt);
	}
	
	virtual void makepacket_info(packet_encoder* packet)
	{
		auto _ptr = m_machine->get_gameuser_info();
		if (_ptr != NULL)
		{
			packet->addmessage(_ptr);
		}
	}

	virtual void makepacket_data(packet_encoder* packet)
	{
		auto _ptr = m_machine->get_gameuser_data();
		if (_ptr != NULL)
		{
			packet->addmessage(_ptr);
		}
	}

};

class gameuser_common : public sc::simple_state < gameuser_common, gameuser_machine, gameuser_live >, istatetype
{
public:
	gameuser_common();
	~gameuser_common();

	// makepacket
	typedef mpl::list< sc::custom_reaction<evpacketinfolist>
		, sc::custom_reaction<evmakedata>
		, sc::custom_reaction<evpacketdatalist>
		, sc::custom_reaction<evtick>
		, sc::custom_reaction<evaddgameobject>
		, sc::custom_reaction<evdeletegameobject>> reactions;



	sc::result react(const evtick &arg_evt);
	sc::result react(const evpacketinfolist &arg_evt);
	sc::result react(const evmakedata &arg_evt);
	sc::result react(const evpacketdatalist &arg_evt);
	sc::result react(const evaddgameobject &arg_evt);
	sc::result react(const evdeletegameobject &arg_evt);

	virtual int getstatetype() const
	{
		return 20;
	}
};

class gameuser_dead : public sc::simple_state < gameuser_dead, gameuser_common > , istatetype
{
	float m_cooltime; 
public:
	gameuser_dead();
	~gameuser_dead();

	typedef mpl::list< sc::custom_reaction<evtick> > reactions;
	sc::result react(const evtick &arg_evt);

	virtual int getstatetype() const
	{
		return 0;
	}

private:

};

class gameuser_live : public sc::simple_state<gameuser_live, gameuser_common, mpl::list< gameuser_idle, gameuser_condition>>, istatetype
{
public:
	
	gameuser_live();
	~gameuser_live();

//	typedef mpl::list< sc::custom_reaction<evtick> > reactions;
//	sc::result react(const evtick &arg_evt);
	virtual int getstatetype() const
	{
		return 20;
	}
};

class gameuser_idle : public sc::simple_state<gameuser_idle, gameuser_live::orthogonal<0> >, istatetype
{
public:
	gameuser_idle();
	~gameuser_idle();
	
	typedef mpl::list< sc::custom_reaction<evtick>, sc::custom_reaction<evmove>, sc::custom_reaction<evskill<skilltype::skill1>>, sc::custom_reaction<evskill<skilltype::skill1>> > reactions;
	sc::result react(const evtick &arg_evt);
	sc::result react(const evmove &arg_evt);

	sc::result react(const evskill<skilltype::skill1> & arg_evt);
	sc::result react(const evskill<skilltype::skill2> & arg_evt);

	virtual int getstatetype() const
	{
		return 1;
	}
};


class gameuser_move : public sc::simple_state<gameuser_move, gameuser_live::orthogonal<0> >, istatetype
{
	evmove m_evmove;
public:
	gameuser_move();
	~gameuser_move();

	typedef mpl::list< sc::custom_reaction<evtick>, sc::custom_reaction<evmove>, sc::custom_reaction<evskill<skilltype::skill1>>, sc::custom_reaction<evskill<skilltype::skill1>> > reactions;
	sc::result react(const evtick &arg_evt);
	sc::result react(const evmove &arg_evt);
	sc::result react(const evskill<skilltype::skill1> & arg_evt);
	sc::result react(const evskill<skilltype::skill2> & arg_evt);

	virtual int getstatetype() const
	{
		return 2;
	}
};
class skill_interface;

template<skilltype _skilltype>
class gameuser_skill : public sc::simple_state<gameuser_skill<_skilltype>, gameuser_live::orthogonal<0> >, istatetype
{
	float m_cooltime;
	skill_interface* m_skillinterface;
public:
	gameuser_skill();
	~gameuser_skill();

	typedef mpl::list< sc::custom_reaction<evtick>, sc::deferral<evmove> > reactions;
	sc::result react(const evtick &arg_evt);

	virtual int getstatetype() const
	{
		return 3;
	}
};




class gameuser_condition : public sc::simple_state<gameuser_condition , gameuser_live::orthogonal<1> >
{
public:
	gameuser_condition();
	~gameuser_condition();

	typedef mpl::list< sc::custom_reaction<evtick> > reactions;
	sc::result react(const evtick &arg_evt);
private:
	

};