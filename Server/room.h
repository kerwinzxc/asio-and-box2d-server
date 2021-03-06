#pragma once
#include "def.h"
#include "body.pb.h"

class room
	:public boost::enable_shared_from_this<room>, b2ContactListener
{
	boost::asio::deadline_timer m_timer;
	boost::asio::strand m_strand;

	ptr_b2world m_world;

	ptr_makeindex m_makeindex;

	boost::asio::time_traits<boost::posix_time::ptime>::time_type m_pretime;


	set<weakptr_user_session> m_userset;

	typedef map<weakptr_user_session, ptr_gameobject> tgameobjectmap;
	tgameobjectmap m_usersessiongameobjectmap;

	typedef map<int ,ptr_gameobject> tgameobjectindexmap;
	tgameobjectindexmap m_gameobjectindexmap;

	
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
	void process_gamemessage(ptr_user_session arg_user, ptr_proto_message arg_message, BYTE arg_type);

	ptr_gameobject get_gameobject(unsigned int arg_gameobjectindex);

	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	bool check_tagstatus(const fixturetag& arg_taga, const fixturetag& arg_tagb, size_t arg_comparea, bool& arg_result);
	bool check_tagstatus(const fixturetag& arg_tag, size_t arg_compare);
	weakptr_gameobject createdumbmob(b2Vec2 arg_positon);
};

