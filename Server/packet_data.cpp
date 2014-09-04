#include "stdafx.h"
#include "packet_data.h"
#include "body.pb.h"

packet_data::packet_data()
{
}


packet_data::~packet_data()
{
}

int packet_data::get_headersize()
{
	return headersize;
}

unsigned char* packet_data::get_header()
{

	//google::protobuf::io::CodedOutputStream stream()
	
	return m_packetdata;
}

unsigned char* packet_data::get_body()
{
	return m_packetdata + headersize;
}
