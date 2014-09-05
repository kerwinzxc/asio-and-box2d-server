#include "stdafx.h"
#include "user_session.h"
#include "packet_data.h"
#include "room_provider.h"
#include "room.h"
#include "packet_decoder.h"
#include "body.pb.h"
#include "user_session_manager.h"
#include "packet_encoder.h"
#include "udpserver.h"

user_session::user_session(boost::asio::ip::tcp::socket arg_socket, ptr_room_provider arg_room_provider)
	:m_socket(std::move(arg_socket))
	, m_strand(arg_socket.get_io_service())
	, m_room_provider(arg_room_provider)
	, m_timer(arg_socket.get_io_service())
	, m_uuid(boost::uuids::nil_uuid())
{
	m_receivedata = boost::make_shared<packet_data>();
	m_packetdecoder = boost::make_shared<packet_decoder>(m_receivedata);
	cout << "start session" << endl;
	
}


user_session::~user_session()
{
	cout << "End session" << endl;
}

void user_session::start()
{
	do_receive();
	
}

void user_session::do_receive()
{
	auto self(shared_from_this());
	setup_timeout();
	m_socket.async_receive( boost::asio::buffer(m_receivedata->get_header(), m_receivedata->get_maxpacketsize()),
		m_strand.wrap([this, self](boost::system::error_code ec, std::size_t length)
	{
		m_packetdecoder->decode_header();
		if (!ec && m_packetdecoder->getbodysize())
		{
			m_packetdecoder->decode_body();
			ptr_proto_message _message;
			BYTE _type;
			while ((_type = m_packetdecoder->getbody(_message)) != 255)
			{
				if (_type == databody::login::descriptor()->index())
				{
					databody::login* _login = (databody::login*)_message.get();
					if (_login->loginstep() == 1)
					{
						m_deviceid = _login->deviceid();
						user_session_manager::getInst().add_tcp_login_user(shared_from_this(), _login->deviceid(), boost::bind(&user_session::handle_add_tcp_user, shared_from_this(), _1));						
					}
					else if (_login->loginstep() == 5)
					{
						m_room_provider->join_room(shared_from_this(), boost::bind(&user_session::handle_join, shared_from_this(), _1));
					}
				}
				if (_type == databody::move::descriptor()->index()
					|| _type == databody::jump::descriptor()->index()
					)
				{
					if (m_room != NULL)
					{
						m_room->process_gamemessage(shared_from_this(), _message, _type);
					}					
				}
			}
			do_receive();
		}
		else
		{
			session_end();
		}
	}));
}

void user_session::do_writequeue(ptr_packet_data arg_packet_data, packet_sendtype arg_type)
{
	if (arg_type == packet_sendtype::tcp)
	{
		m_qacketqueue.push(arg_packet_data);
		do_sendpacket();
	}
	else
	{
		m_udpserver->do_writequeue(m_endpoint, arg_packet_data);
	}

	
}

void user_session::do_sendpacket()
{
	ptr_packet_data data;
	auto self(shared_from_this());
	if (m_senddata.exchange(true) == true)
	{
		return;
	}
	if (m_qacketqueue.try_pop(data))
	{
		boost::asio::async_write(m_socket, boost::asio::buffer(data->get_header(), data->get_headersize() + data->get_bodysize()),
			m_strand.wrap([this, self](boost::system::error_code ec,std::size_t bytes_transferred)
		{
			if (!ec)
			{
				m_senddata = false;
				do_sendpacket();
			}
			else
			{
				session_end();
			}
			
		}));
	}
	else
	{
		m_senddata = false;
	}
}

void user_session::session_end()
{	
	user_session_manager::getInst().delete_user(m_uuid);
	m_socket.close();
	m_timer.cancel();

	if (m_room != NULL)
	{
		m_room_provider->exit_room(shared_from_this(), m_room,bind(&user_session::handle_exit,shared_from_this()));
	}
}

void user_session::handle_join(ptr_room arg_room)
{
	m_room = arg_room;
	int i = 0;
	i = 10;
}

void user_session::handle_exit()
{
	int i = 0;
	i = 10;
}

void user_session::setup_timeout()
{
	m_timer.cancel();
	m_timer.expires_from_now(boost::posix_time::milliseconds(10000));
	m_timer.async_wait(m_strand.wrap(boost::bind(&user_session::timeout, shared_from_this())));
}



void user_session::timeout()
{
//	m_socket.close();
}


void user_session::handle_add_tcp_user(boost::uuids::uuid arg_uuid)
{
	auto self(shared_from_this());
	m_strand.dispatch([&,this, self, arg_uuid]()
	{
		m_uuid = arg_uuid;
		databody::login* _sendlogin = new databody::login();

		_sendlogin->set_allocated_deviceid(&m_deviceid);
		_sendlogin->set_loginstep(2);
		string u = to_string(m_uuid);
		_sendlogin->set_allocated_uuid(&u);

		ptr_packet_data data = boost::make_shared<packet_data>();
		ptr_packet_encoder encoder = boost::make_shared<packet_encoder>(data);
		encoder->addmessage(_sendlogin);
		encoder->makeheader();

		do_writequeue(data, packet_sendtype::tcp);
	});
	
}

void user_session::add_endpoint(ptr_udpserver arg_udpserver, udp::endpoint arg_endpoint)
{
	m_udpserver = arg_udpserver;
	m_endpoint = arg_endpoint;
}