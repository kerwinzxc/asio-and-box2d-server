#pragma once
#include "def.h"
class server
{
protected:

	static int m_serverport;
	static int m_threadcount;

private:
	boost::asio::io_service m_io;
	boost::asio::ip::tcp::acceptor m_acceptor;
	boost::asio::ip::tcp::socket m_tcpsocket;

	

	ptr_room_provider m_roomprovider;

public:
	server(int tcpport , int udpport);
	~server();
	void run();
	void do_accept();

	void init_singgleton();

	
};

