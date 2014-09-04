#include "stdafx.h"
#include "user_session_manager.h"
#include "user_session.h"
#include "udpserver.h"

user_session_manager::user_session_manager()
{
}


user_session_manager::~user_session_manager()
{
}

void user_session_manager::init(boost::asio::io_service& arg_io, ptr_udpserver arg_udpserver)
{
	m_udpserver = arg_udpserver;
	m_strand = boost::make_shared<boost::asio::strand>(arg_io);
}


void user_session_manager::delete_user(boost::uuids::uuid arg_uuid)
{
	m_strand->dispatch([&, arg_uuid]()
	{
		m_set.erase(arg_uuid);
	});
}


void user_session_manager::add_tcp_login_user(ptr_user_session arg_user_session, string arg_deviceid, boost::function<void(boost::uuids::uuid)> arg_handler)
{
	m_strand->dispatch([&, arg_user_session, arg_deviceid]()
	{
		
		boost::uuids::uuid u = gen(arg_deviceid);
		m_set.insert(userdata(u, arg_user_session));
		arg_handler(u);
	});
}


void user_session_manager::add_udp_login_user(udp::endpoint arg_endpoint, boost::uuids::uuid arg_uuid, boost::function<void(udp::endpoint)> arg_handler)
{
	m_strand->dispatch([&, arg_endpoint, arg_uuid]()
	{
		auto it = m_set.find(arg_uuid);
		if (it != m_set.end())
		{
			m_set.modify(it, boost::bind(&set_endpoint, _1, arg_endpoint));
			it->m_user_session->add_endpoint(m_udpserver,arg_endpoint);
			arg_handler(arg_endpoint);
		}
	});
}