#pragma once
using namespace std;
using boost::asio::ip::tcp;
using boost::asio::ip::udp;



//
//namespace databody
//{
//	class gameuser_data;
//	class gameuser_info;
//	class dumbmob_data;
//	class dumbmob_info;
//	class staticobject_info;
//	class login;
//	class join;
//	class exit;
//	class move;
//	class jump;
//	class skill1;
//	class commandresult;
//}

namespace google
{
	namespace protobuf
	{
		class Message;
	}
}


class user_session;
class server;
class room_provider;
class room;
class state_unit;

class packet_encoder;
class packet_data;
class packet_decoder;

class gameuser_machine;
class staticobject_machine;
class dumbmob_machine;
class potal_machine;
class regenerationmob_machine;
class basemob_machine;

class gameobject;

class util_makeindex;
class udpserver;

#define roommaxuser 20

typedef boost::shared_ptr < user_session > ptr_user_session;
typedef boost::shared_ptr < packet_data > ptr_packet_data;
typedef boost::shared_ptr < packet_decoder > ptr_packet_decoder;
typedef boost::shared_ptr < packet_encoder > ptr_packet_encoder;

typedef boost::shared_ptr< room_provider > ptr_room_provider;
typedef boost::shared_ptr< room > ptr_room;
typedef boost::shared_ptr< state_unit > ptr_state_unit;
typedef boost::shared_ptr< b2World > ptr_b2world;

typedef boost::shared_ptr< gameobject > ptr_gameobject;
typedef boost::shared_ptr< util_makeindex > ptr_makeindex;
typedef boost::shared_ptr< google::protobuf::Message > ptr_proto_message;
typedef boost::shared_ptr< boost::asio::strand > ptr_strand;
typedef boost::shared_ptr< udpserver > ptr_udpserver;

typedef boost::shared_ptr< gameuser_machine > ptr_gameuser_machine;
typedef boost::shared_ptr< staticobject_machine > ptr_staticobject_machine;
typedef boost::shared_ptr< dumbmob_machine > ptr_dumbmob_machine;
typedef boost::shared_ptr< potal_machine > ptr_potal_machine;
typedef boost::shared_ptr< regenerationmob_machine > ptr_regenerationmob_machine;
typedef boost::shared_ptr< basemob_machine > ptr_basemob_machine;

typedef boost::weak_ptr < user_session > weakptr_user_session;
typedef boost::weak_ptr < room > weakptr_room;
typedef boost::weak_ptr < gameobject > weakptr_gameobject;

struct fixturetag{
	fixturetag(unsigned long _value)
		: m_bitset(_value)
	{
		m_bitset.to_ulong();
	}

	fixturetag()
		: m_bitset(0)
	{
	}
	fixturetag(const fixturetag& _value)
	{
		m_bitset = _value.m_bitset;
	}

	std::bitset<32> m_bitset;

	void reset()
	{
		m_bitset.reset();
	}
	bool getoption(size_t _pos)const
	{
		return m_bitset.test(_pos);
	}
	void setoption(size_t _pos,bool _value)
	{
		m_bitset.set(_pos, _value);
	}
	unsigned long getvalue()const
	{
		return m_bitset.to_ulong();
	}
	
};

#define gameobjectindextag_gameuser 1
#define gameobjectindextag_staticobject 5001




#define FixtureTag_Body 0 // body
#define FixtureTag_Wire 1 //


#define FixtureTag_SightRader 2 // body
#define FixtureTag_AttackRader 3 // body
#define FixtureTag_PotalRader 4 // body




#define FixtureTag_Gameuser 10
#define FixtureTag_Mob 11
#define FixtureTag_Npc 12
#define FixtureTag_Ground 13


enum packet_sendtype{ tcp , udp};

#include "interface.h"
#include "multi_index.h"