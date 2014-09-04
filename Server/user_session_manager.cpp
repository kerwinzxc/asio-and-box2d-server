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

void user_session_manager::delete_user(ptr_user_session arg_user_session)
{
	m_strand->dispatch([&, arg_user_session]()
	{
		m_user_session_set.erase(arg_user_session);
	});
}
