#include "stdafx.h"
#include "udpserver.h"
#include "user_session_manager.h"


udpserver::udpserver(boost::asio::io_service& arg_io, int arg_port)
	:m_udpsocket(arg_io, udp::endpoint(udp::v4(),arg_port))
	, m_strand(arg_io)
{
	m_data = boost::make_shared<packet_data>();
	do_receive();
}


udpserver::~udpserver()
{
}

void udpserver::do_receive()
{
	m_udpsocket.async_receive_from(
		boost::asio::buffer(m_data->get_header(), m_data->get_maxpacketsize()), sender_endpoint_,
		m_strand.wrap([this](boost::system::error_code ec, std::size_t bytes_recvd)
	{
		if (!ec && bytes_recvd > 0)
		{
			packet_decoder _packet(m_data);
			_packet.decode_header();
			_packet.decode_body();
			ptr_proto_message _message;
			BYTE _type;
			while ((_type = _packet.getbody(_message)) != 255)
			{
				if (_type == databody::login::descriptor()->index())
				{
					databody::login* p = (databody::login*)_message.get();

					boost::uuids::uuid u = boost::lexical_cast<boost::uuids::uuid>(p->uuid());
					user_session_manager::getInst().add_udp_login_user(sender_endpoint_, u,boost::bind(&udpserver::handle_add_udp_login_user,shared_from_this(),_1));

				}

				BYTE _type = _packet.getbody(_message);
			}
		}
		else
		{
			do_receive();
		}
	}));
}

void udpserver::handle_add_udp_login_user(udp::endpoint arg_endpoint)
{
	auto self(shared_from_this());
	m_strand.dispatch([&, this, self]()
	{
		databody::login* _sendlogin = new databody::login();
		_sendlogin->set_loginstep(4);
		ptr_packet_data data = boost::make_shared<packet_data>();
		ptr_packet_encoder encoder = boost::make_shared<packet_encoder>(data);
		encoder->addmessage(_sendlogin);
		encoder->makeheader();

		do_writequeue(arg_endpoint, data);
	});
}

void udpserver::do_writequeue(udp::endpoint arg_dest,ptr_packet_data _data)
{
	cout << "do_writequeue" << endl;
	m_udpsocket.async_send_to(
		boost::asio::buffer(_data->get_header(), _data->get_headersize() + _data->get_bodysize()), arg_dest,
		[this](boost::system::error_code ec, std::size_t bytes_sent)
	{
		cout << "byte_sent"<<bytes_sent<<endl;
	});


}
