#include "stdafx.h"
#include "user_session_manager.h"


tbb::atomic<user_session_manager *> user_session_manager::inst;
user_session_manager::user_session_manager()
{
}


user_session_manager::~user_session_manager()
{
}

void user_session_manager::init(boost::asio::io_service& arg_io)
{
	m_strand = boost::make_shared<boost::asio::strand>(arg_io);
}

void user_session_manager::add_user(ptr_user_session arg_user_session)
{
	m_strand->dispatch([&,arg_user_session]()
	{
		m_user_session_set.insert(arg_user_session);

		
	});	
}

void user_session_manager::delete_user(ptr_user_session arg_user_session ,boost::uuids::uuid arg_uuid)
{
	m_strand->dispatch([&, arg_user_session,arg_uuid]()
	{
		m_user_session_set.erase(arg_user_session);
		m_uid_user_session_map.erase(arg_uuid);
		m_uid_endpoint.erase(arg_uuid);
	});
}


void user_session_manager::add_tcp_login_user(ptr_user_session arg_user_session, string arg_deviceid, boost::function<void(boost::uuids::uuid)> arg_handler)
{
	m_strand->dispatch([&, arg_user_session, arg_deviceid]()
	{
		boost::uuids::uuid u = gen(arg_deviceid);
		m_uid_user_session_map.insert(map<boost::uuids::uuid, ptr_user_session>::value_type(u, arg_user_session));
		arg_handler(u);
	});
}


void user_session_manager::add_udp_login_user(udp::endpoint arg_endpoint, boost::uuids::uuid arg_deviceid, boost::function<void(udp::endpoint)> arg_handler)
{
	m_strand->dispatch([&, arg_endpoint, arg_deviceid]()
	{
		m_uid_endpoint.insert(map<boost::uuids::uuid, udp::endpoint>::value_type(arg_deviceid, arg_endpoint));
		arg_handler(arg_endpoint);
	});
}

