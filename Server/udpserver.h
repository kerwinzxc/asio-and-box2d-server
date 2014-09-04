#pragma once
#include "def.h"
#include "body.pb.h"
#include "packet_data.h"
#include "packet_decoder.h"
#include "packet_encoder.h"

struct udppacket
{
	udppacket()
	{

	}
	udppacket(ptr_packet_data arg_packet_data, udp::endpoint arg_endpoint)
	{
		m_packet_data = arg_packet_data;
		m_endpoint = arg_endpoint;
	}
	ptr_packet_data m_packet_data;
	udp::endpoint m_endpoint;
};
class udpserver
	:public boost::enable_shared_from_this<udpserver>
{
	udp::socket m_udpsocket;
	udp::endpoint sender_endpoint_;
	ptr_packet_data m_data;
	boost::asio::strand m_strand;

	tbb::concurrent_queue<udppacket> m_packetqueue;
	std::atomic<bool> m_senddata;
public:
	udpserver(boost::asio::io_service& arg_io, int arg_port);
	~udpserver();

	void do_receive();
	void handle_add_udp_login_user(udp::endpoint arg_endpoint);
	void do_writequeue(udp::endpoint arg_dest, ptr_packet_data _data);
	void do_sendpacket();
};

