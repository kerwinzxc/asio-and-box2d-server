#pragma once
#include "def.h"


static const int maxpacketsize = 2048;
static const int headersize = 4;
static const int maxbodysize = 2044;

class packet_data
{
	unsigned char m_packetdata[maxpacketsize];

	unsigned int bodysize;


	//vector< google::protobuf::Message > m_vec;
public:
	packet_data();
	~packet_data();

	unsigned char* get_header();

	unsigned char* get_body();

	int get_headersize();
	
	unsigned int get_bodysize() const { return bodysize; }
	void set_bodysize(unsigned int val) { bodysize = val; }

	int get_maxbodysize()
	{
		return maxbodysize;
	}

	int get_maxpacketsize()
	{
		return maxpacketsize;
	}


};

