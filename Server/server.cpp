#include "stdafx.h"
#include "Server.h"
#include "user_session.h"
#include "room_provider.h"
#include "user_session_manager.h"
#include "udpserver.h"

int server::m_threadcount = 5;

server::server(int tcpport, int udpport)
	: m_acceptor(m_io, tcp::endpoint(boost::asio::ip::tcp::v4(), tcpport))
	, m_tcpsocket(m_io)
{
	m_udpserver = boost::make_shared<udpserver>(m_io, udpport);
	m_roomprovider = boost::make_shared<room_provider>(m_io);
	//m_roomprovider->init_rooms();
	do_accept();


}


server::~server()
{
}


void server::run()
{
	init_singgleton();


	std::vector<boost::shared_ptr<boost::thread>> threads;

	for (int i = 0; i < m_threadcount; i++)
	{
		boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&boost::asio::io_service::run, &m_io)));
		threads.push_back(thread);
	}

	for (int i = 0; i < m_threadcount; i++)
	{
		threads[i]->join();
	}
}


void server::do_accept()
{
	m_acceptor.async_accept(m_tcpsocket, [this](boost::system::error_code ec)
	{
		if (!ec)
		{
			boost::make_shared<user_session>(std::move(m_tcpsocket),m_roomprovider)->start();
		}
		do_accept();
	});

}

void server::init_singgleton()
{
	user_session_manager::getInst().init(m_io,m_udpserver);
}


