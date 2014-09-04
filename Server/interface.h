#pragma once

class gameobject
{
public:
	gameobject(){};
	~gameobject(){};

	virtual void initiate() = 0;
	virtual void process_event(const sc::event_base & evt) = 0;
	virtual void makepacket_info(packet_encoder* packet){}
	virtual void makepacket_data(packet_encoder* packet){}
private:

};
