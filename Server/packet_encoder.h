#pragma once
#include "def.h"


class packet_encoder
{
	std::list< google::protobuf::Message*> m_messagelist;

	ptr_packet_data m_packetdata;
	google::protobuf::io::ZeroCopyOutputStream* m_bodyarray;
	google::protobuf::io::CodedOutputStream* m_bodycoded;
	google::protobuf::io::ArrayOutputStream* m_headerarray;
	google::protobuf::io::CodedOutputStream* m_headercoded;

public:
	packet_encoder(ptr_packet_data arg_packet);
	~packet_encoder();

	void addmessage(google::protobuf::Message* arg_message);

	
	void makeheader();


};

