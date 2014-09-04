#include "stdafx.h"
#include "udpserver.h"


udpserver::udpserver(boost::asio::io_service& arg_io, int arg_port)
	:m_udpsocket(arg_io, udp::endpoint(udp::v4(),arg_port))
{
	m_data = boost::make_shared<packet_data>();
}


udpserver::~udpserver()
{
}

void udpserver::do_receive()
{
	m_udpsocket.async_receive_from(
		boost::asio::buffer(m_data->get_header(), m_data->get_maxpacketsize()), sender_endpoint_,
		[this](boost::system::error_code ec, std::size_t bytes_recvd)
	{
		if (!ec && bytes_recvd > 0)
		{
			packet_decoder _packet(m_data);
			_packet.decode_header();
			_packet.decode_body();
			ptr_proto_message _message;
			BYTE _type = _packet.getbody(_message);
			while (_type != 255)
			{
				if (_type == databody::login::descriptor()->index())
				{
					databody::login* p = (databody::login*)_message.get();


				}

				BYTE _type = _packet.getbody(_message);
			}
		}
		else
		{
			do_receive();
		}
	});
}
