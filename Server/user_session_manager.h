#pragma once
#include "def.h"

class user_session_manager
{
	ptr_strand m_strand;
	set<ptr_user_session> m_user_session_set;
	boost::uuids::string_generator gen;
	userdata_set m_set;

	ptr_udpserver m_udpserver;
	

public:
	user_session_manager();
	~user_session_manager();

	static user_session_manager &getInst() {
		static user_session_manager _inst;
		return _inst;
	}

	void init(boost::asio::io_service& arg_io,ptr_udpserver arg_udpserver);
	void delete_user(boost::uuids::uuid arg_uuid);
	void add_tcp_login_user(ptr_user_session arg_user_session, string arg_deviceid, boost::function<void(boost::uuids::uuid)> arg_handler);
	void add_udp_login_user(udp::endpoint arg_endpoint, boost::uuids::uuid arg_deviceid, boost::function<void(udp::endpoint)> arg_handler);
};