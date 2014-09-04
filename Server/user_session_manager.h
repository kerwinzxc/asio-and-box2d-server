#pragma once
#include "def.h"

class user_session_manager
{
	ptr_strand m_strand;
	set<ptr_user_session> m_user_session_set;

	map<int, ptr_user_session> m_uid_user_session_map;
	map<int, udp::endpoint> m_uid_endpoint;
	static tbb::atomic<user_session_manager*> inst;

	boost::uuids::uuid u;

public:
	user_session_manager();
	~user_session_manager();

	static user_session_manager &getInst() {
			if (inst == 0) {
				user_session_manager* temp = new user_session_manager();
				if (inst.compare_and_swap(temp, NULL) != NULL) {
					delete temp;
				}
			}
			return *inst;
	}

	void init(boost::asio::io_service& arg_io);
	void add_user(ptr_user_session arg_user_session);
	void delete_user(ptr_user_session arg_user_session);
};

