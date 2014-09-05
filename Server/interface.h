#pragma once
#include "def.h"


class gameobject
{
protected:
	unsigned int m_gameobjectindex;

	ptr_makeindex m_makeindex;
public:
	gameobject(ptr_makeindex arg_makeindex);
	virtual ~gameobject();

	virtual void initiate() = 0;
	virtual void process_event(const sc::event_base & evt) = 0;
	virtual void makepacket_info(packet_encoder* packet){}
	virtual void makepacket_data(packet_encoder* packet){}

	unsigned int get_gameobjectindex() const { return m_gameobjectindex; }
private:

};
