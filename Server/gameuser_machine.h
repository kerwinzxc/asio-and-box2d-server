#pragma once
#include "def.h"
#include "event.h"
#include "body.pb.h"
#include "packet_encoder.h"
#include "query.h"


class gameuser_live;
class gameuser_idle;
class gameuser_move;
class gameuser_skill1;
class gameuser_skill2;

class gameuser_condition;// 상태이상 , stat 변경,

class gameuser_dead;
class gameuser_common;

class gameuser_machine : public sc::state_machine < gameuser_machine, gameuser_common >
{

public:
	float m_skillcooltime[2];
	float m_revivedtime;
	float m_movespeed;
	float m_curplayerhp;
	float m_maxplayerhp;
	float m_swordangle;

	databody::movedirectiontype m_directiontype;

	unsigned int m_gameobjectindex;
	gameobject* m_gameobject;

	int m_StateType;
	

	b2Body* m_body;
	ptr_b2world m_world;
	weakptr_room m_room;

	databody::gameuser_info* m_info;
	databody::gameuser_data* m_data;

	databody::gameuser_info* get_gameuser_info() const;
	databody::gameuser_data* get_gameuser_data() const;

	map<weakptr_gameobject,int> m_infolist;
	map<weakptr_gameobject,int> m_datalist;
	map<weakptr_gameobject, int> m_deletelist;


	gameuser_machine(weakptr_room arg_room, ptr_b2world arg_world, unsigned arg_gameobjectindex);
	~gameuser_machine();

	void makepacket_gameuser_info();

	void makepacket_gameuser_data();
	void deletepacket_gameuser_data();

	void addgameobject(ptr_gameobject arg_gameobject)
	{
		m_infolist.insert(map<weakptr_gameobject, int>::value_type(arg_gameobject, arg_gameobject->get_gameobjectindex()));
		m_datalist.insert(map<weakptr_gameobject, int>::value_type(arg_gameobject, arg_gameobject->get_gameobjectindex()));
	}
	void deletegameobject(ptr_gameobject arg_gameobject)
	{
		m_infolist.erase(arg_gameobject);
		m_datalist.erase(arg_gameobject);
		m_deletelist.insert(map<weakptr_gameobject, int>::value_type(arg_gameobject, arg_gameobject->get_gameobjectindex()));
	}

	void SetStateType(int val) { m_StateType = val; }

	void raycast(raycastcallback* callback, const float& angle, const float& arg_length) const;
	
};

class gameuser : public gameobject, public boost::enable_shared_from_this < gameuser >
{
public:
	ptr_gameuser_machine m_machine;
	ptr_b2world m_world;
	ptr_makeindex m_makeindex;
	bool isprocess = false;
	weakptr_room m_room;

	gameuser(weakptr_room arg_room, ptr_b2world arg_world, ptr_makeindex arg_makeindex)
		: m_room(arg_room)
		, m_world(arg_world)
		, m_makeindex(arg_makeindex)
		, gameobject(arg_makeindex)
	{
		m_machine = boost::make_shared<gameuser_machine>(m_room, m_world, m_gameobjectindex);
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
		auto _ptr = m_machine->get_gameuser_info();
		_ptr->set_control(control);
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

class gameuser_common : public sc::simple_state < gameuser_common, gameuser_machine, gameuser_live >
{
public:
	gameuser_common();
	~gameuser_common();

	// makepacket
	typedef mpl::list< sc::custom_reaction<evmakepacketdata>
		, sc::custom_reaction<evmakedata>
		, sc::custom_reaction<evtick>
		, sc::custom_reaction<evaddgameobject>
		, sc::custom_reaction<evdeletegameobject>
		, sc::custom_reaction<evforcemove> > reactions;



	sc::result react(const evtick &arg_evt);
	sc::result react(const evmakepacketdata &arg_evt);
	sc::result react(const evmakedata &arg_evt);
	sc::result react(const evaddgameobject &arg_evt);
	sc::result react(const evdeletegameobject &arg_evt);
	sc::result react(const evforcemove &arg_evt);

};

class gameuser_dead : public sc::simple_state < gameuser_dead, gameuser_common > 
{
	float m_cooltime; 
public:
	gameuser_dead();
	~gameuser_dead();

	typedef mpl::list< sc::custom_reaction<evtick> > reactions;
	sc::result react(const evtick &arg_evt);


private:

};

class gameuser_live : public sc::simple_state<gameuser_live, gameuser_common, mpl::list< gameuser_idle, gameuser_condition>>
{
public:
	
	gameuser_live();
	~gameuser_live();

};

class gameuser_idle : public sc::simple_state<gameuser_idle, gameuser_live::orthogonal<0> >
{
public:
	gameuser_idle();
	~gameuser_idle();
	
	typedef mpl::list< sc::custom_reaction<evtick>, sc::custom_reaction<evmove>, sc::custom_reaction<evjump>, sc::custom_reaction<evskill>> reactions;
	sc::result react(const evtick &arg_evt);
	sc::result react(const evmove &arg_evt);
	sc::result react(const evjump &arg_evt);
	sc::result react(const evskill & arg_evt);
};


class gameuser_move : public sc::simple_state<gameuser_move, gameuser_live::orthogonal<0> >
{
	evmove m_evmove;
	bool m_jumped;
public:
	gameuser_move();
	~gameuser_move();

	typedef mpl::list< sc::custom_reaction<evtick>, sc::custom_reaction<evmove>, sc::custom_reaction<evjump>, sc::custom_reaction<evskill> > reactions;
	sc::result react(const evtick &arg_evt);
	sc::result react(const evmove &arg_evt);
	sc::result react(const evjump &arg_evt);
	sc::result react(const evskill & arg_evt);

};

class gameuser_skill1 : public sc::simple_state<gameuser_skill1, gameuser_live::orthogonal<0> >
{
	float m_cooltime;
	float m_angle;

	bool init;
	bool loop;
	bool end;
	bool hit;

public:
	gameuser_skill1();
	~gameuser_skill1();

	typedef mpl::list< sc::custom_reaction<evtick>, sc::deferral<evmove> > reactions;
	sc::result react(const evtick &arg_evt);

	virtual int getstatetype() const
	{
		return 3;
	}
};


class gameuser_skill2 : public sc::simple_state<gameuser_skill2, gameuser_live::orthogonal<0> >
{
	float m_cooltime;
public:
	gameuser_skill2();
	~gameuser_skill2();

	typedef mpl::list< sc::custom_reaction<evtick>, sc::deferral<evmove> > reactions;
	sc::result react(const evtick &arg_evt);

};




class gameuser_condition : public sc::simple_state<gameuser_condition , gameuser_live::orthogonal<1> >
{
	float m_hitcooldown;
public:
	gameuser_condition();
	~gameuser_condition();

	typedef mpl::list< sc::custom_reaction<evtick>
	,sc::custom_reaction<evhit> > reactions;
	sc::result react(const evtick &arg_evt);
	sc::result react(const evhit &arg_evt);
private:
	

};