#pragma once
using namespace std;
using boost::asio::ip::tcp;
using boost::asio::ip::udp;




namespace databody
{
	class gameuser_data;
	class gameuser_info;
	class staticobject_info;
	class login;
	class join;
	class exit;
	class move;
	class jump;
	class skill1;
	class commandresult;
}

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
typedef boost::shared_ptr< gameuser_machine > ptr_gameuser_machine;
typedef boost::shared_ptr< staticobject_machine > ptr_staticobject_machine;
typedef boost::shared_ptr< gameobject > ptr_gameobject;
typedef boost::shared_ptr< util_makeindex > ptr_makeindex;
typedef boost::shared_ptr< google::protobuf::Message > ptr_proto_message;
typedef boost::shared_ptr< boost::asio::strand > ptr_strand;
typedef boost::shared_ptr< udpserver > ptr_udpserver;


typedef boost::weak_ptr < user_session > weakptr_user_session;
typedef boost::weak_ptr < room > weakptr_room;
typedef boost::weak_ptr < gameobject > weakptr_gameobject;


#define gameobjectindextag_gameuser 1
#define gameobjectindextag_staticobject 5001




#define FixtureTag_GameuserBody 1
#define FixtureTag_GameuserBodyNearRader1 2 // 이레이더로 근처의 사물들을 체크 한다.
#define FixtureTag_GameuserBodyRader2 3
#define FixtureTag_GameuserBodyRader3 4
#define FixtureTag_GameuserBodyRader4 5
#define FixtureTag_MapObject 6

enum packet_sendtype{ tcp , udp};

#include "interface.h"
#include "multi_index.h"