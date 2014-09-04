#pragma once
#include "def.h"

class room
	:public boost::enable_shared_from_this<room>, b2ContactListener
{
	boost::asio::deadline_timer m_timer;
	boost::asio::strand m_strand;

	ptr_b2world m_world;

	ptr_makeindex m_makeindex;

	boost::asio::time_traits<boost::posix_time::ptime>::time_type m_pretime;


	std::set<weakptr_user_session> m_userset;

	typedef std::map<weakptr_user_session, ptr_gameobject> tgameobjectmap;
	tgameobjectmap m_gameobjectmap;

	typedef std::set<ptr_gameobject> tgameobjectset;
	tgameobjectset m_gameobjectset;

	
public:
	room(boost::asio::io_service& _io);
	~room();

	bool maxroomuser()
	{
		return false;
	}
	
	void start();
	void update();
	void dispatch_join(ptr_user_session arg_user, boost::function<void(ptr_room)> arg_roomprovider_handler, boost::function<void(ptr_room)> arg_user_handler);
	void dispatch_exit(ptr_user_session arg_user, boost::function<void(ptr_room)> arg_roomprovider_handler, boost::function<void()> arg_user_handler);


	virtual void BeginContact(b2Contact* contact);

	/// Called when two fixtures cease to touch.
	virtual void EndContact(b2Contact* contact);

	
};

