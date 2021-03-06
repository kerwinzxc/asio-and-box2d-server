#pragma once
#include "body.pb.h"


enum skilltype{ skill1, skill2 };
struct evtick : sc::event < evtick >
{
	evtick(float arg_tick,bool* arg_destory)
		:m_tick(arg_tick)
		, m_destory(arg_destory)
	{}
	float m_tick;
	bool* m_destory;
};

struct evmove : sc::event < evmove > {
	databody::movedirectiontype m_type;
	evmove(databody::movedirectiontype arg_type)
		:m_type(arg_type)
	{
	}
};
struct evskill : sc::event < evskill > {
	skilltype m_skilltype;
	evskill(skilltype arg_skilltype)
		:m_skilltype(arg_skilltype)
	{
	}
};
struct evjump : sc::event < evjump > {};
struct evland : sc::event < evland > {};


struct evmakedata : sc::event < evmakedata > {};

struct evmakepacketdata : sc::event < evmakepacketdata > {
	packet_encoder* m_tcppacket;
	packet_encoder* m_udppacket;
	evmakepacketdata(packet_encoder* arg_tcppacket, packet_encoder* arg_udppacket)
		: m_tcppacket(arg_tcppacket)
		, m_udppacket(arg_udppacket)
	{

	}
};


struct evaddgameobject : sc::event < evaddgameobject > 
{
	ptr_gameobject m_gameobject;
	evaddgameobject(ptr_gameobject arg_gameobject)
		:m_gameobject(arg_gameobject)
	{
		
	}
};


struct evdeletegameobject : sc::event < evdeletegameobject >
{
	ptr_gameobject m_gameobject;
	evdeletegameobject(ptr_gameobject arg_gameobject)
		:m_gameobject(arg_gameobject)
	{

	}
};

struct evhit : sc::event< evhit >
{
	float m_dameage;
	databody::movedirectiontype dir;
	evhit(float arg_dameage,databody::movedirectiontype arg_dir)
		: m_dameage(arg_dameage)
		, dir(arg_dir)
	{

	}
};

struct evcontact : sc::event < evcontact >
{
	ptr_gameobject m_obj;
	evcontact(ptr_gameobject arg_obj)
		:m_obj(arg_obj)
	{
	}
};

struct evforcemove : sc::event < evcontact >
{
	b2Vec2 m_dest;
	evforcemove(b2Vec2 arg_dest)
		:m_dest(arg_dest)
	{
	}
};