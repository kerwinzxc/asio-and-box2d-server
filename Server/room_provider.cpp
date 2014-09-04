#include "stdafx.h"
#include "room_provider.h"
#include "room.h"

const int room_provider::roomscount = 1;


room_provider::room_provider(boost::asio::io_service& arg_io)
	:m_strand(arg_io)
{
	init_rooms();

	
}


room_provider::~room_provider()
{
}

void room_provider::init_rooms()
{
	for (int i = 0; i < roomscount; i++)
	{
		ptr_room a = boost::make_shared< room >(m_strand.get_io_service());
		a->start();
		m_idleroomsqueue.push_back(a);
	}
}

void room_provider::join_room(ptr_user_session arg_user, boost::function<void(ptr_room)> arg_handler)
{
	m_strand.dispatch([&, arg_user, arg_handler]()
	{
		bool check = false;
		if (m_userset.find(arg_user) == m_userset.end())
		{
			if (m_runroomlist.empty() == false)
			{
				ptr_room ptr = m_runroomlist.front();
				m_runroomlist.pop_front();

				ptr_room _cur = ptr;
				if (ptr != NULL)
				{
					do
					{
						if (_cur->maxroomuser() == false)
						{
							_cur->dispatch_join(arg_user, bind(&room_provider::handle_join_room, shared_from_this(), _1), arg_handler);
							check = true;
							m_runroomlist.push_back(_cur);
							m_userset.insert(arg_user);
							break;
						}

						m_runroomlist.push_back(_cur);
						_cur = m_runroomlist.front();
						m_runroomlist.pop_front();

					} while (ptr != _cur);
				}
			}
			if (check == false)
			{
				ptr_room data = m_idleroomsqueue.front();
				m_idleroomsqueue.pop_front();
				if (data != NULL)
				{
					data->dispatch_join(arg_user, bind(&room_provider::handle_join_room, shared_from_this(), _1), arg_handler);
					check = true;
					m_runroomlist.push_back(data);
					m_userset.insert(arg_user);

				}
			}
		}
	});
}

void room_provider::exit_room(ptr_user_session arg_user, ptr_room arg_room , boost::function<void()> arg_handler)
{
	m_strand.dispatch([&, arg_user, arg_room, arg_handler]()
	{
		arg_room->dispatch_exit(arg_user, bind(&room_provider::handle_exit_room, shared_from_this(), _1), arg_handler);
	});
}

void room_provider::handle_exit_room(ptr_room arg_room)
{
	m_strand.dispatch([&, arg_room]()
	{
		int i = 0;
		i++;
	});
}

void room_provider::handle_join_room(ptr_room arg_room)
{
	m_strand.dispatch([&, arg_room]()
	{
	});
}


