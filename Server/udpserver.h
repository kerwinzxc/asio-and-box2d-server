#pragma once
#include "def.h"
#include "body.pb.h"
#include "packet_data.h"
#include "packet_decoder.h"
#include "packet_encoder.h"
class udpserver
{
	udp::socket m_udpsocket;
	udp::endpoint sender_endpoint_;
	ptr_packet_data m_data;
public:
	udpserver(boost::asio::io_service& arg_io, int arg_port);
	~udpserver();

	void do_receive();
};

