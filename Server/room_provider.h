#pragma once
#include "def.h"


class room_provider
	:public boost::enable_shared_from_this<room_provider>
{
	boost::asio::strand m_strand;

	std::deque<ptr_room> m_idleroomsqueue;
	std::deque<ptr_room> m_runroomlist;
	static const int roomscount;

	std::set<weakptr_user_session> m_userset;
	

public:
	room_provider(boost::asio::io_service& _io);
	~room_provider();

	void init_rooms();
	void join_room(ptr_user_session arg_user, boost::function<void(ptr_room)> handler);
	void exit_room(ptr_user_session arg_user, ptr_room arg_room, boost::function<void()> arg_handler);
	void handle_join_room(ptr_room arg_room);
	void handle_exit_room(ptr_room arg_room);

};

