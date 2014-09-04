#pragma once

#include "def.h"
#include "body.pb.h"

class packet_decoder
{
	ptr_packet_data m_packetdata;

	int m_bodysize;
	google::protobuf::io::ZeroCopyInputStream* m_bodyarray;
	google::protobuf::io::CodedInputStream* m_bodycoded;
	google::protobuf::io::ArrayInputStream* m_headerarray;
	google::protobuf::io::CodedInputStream* m_headercoded;

public:
	packet_decoder(ptr_packet_data arg_data);

	unsigned int getbodysize(); // bodysize

	void decode_header();
	void decode_body();
	BYTE getbody(ptr_proto_message& arg_protomessage); // return: type, data: (message)
	~packet_decoder();
	

};

