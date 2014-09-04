#include "def.h"


class user_session
	:public boost::enable_shared_from_this<user_session>
{
private:

	boost::asio::deadline_timer m_timer;
	tcp::socket m_socket;

	
	ptr_packet_data m_receivedata;
	ptr_packet_decoder m_packetdecoder;

	boost::asio::strand m_strand;

	tbb::concurrent_queue<ptr_packet_data> m_qacketqueue;
	std::atomic<bool> m_senddata;

	ptr_room_provider m_room_provider;
	ptr_room m_room;

	string m_deviceid;
	boost::uuids::uuid m_uuid;

public:
	user_session(boost::asio::ip::tcp::socket _socket,ptr_room_provider arg_room_provider);
	~user_session();
	void start();
	void session_end();
	void do_receive();
	void do_writequeue(ptr_packet_data _senddata);
	void do_sendpacket();
	void handle_join(ptr_room arg_room);
	void handle_exit();
	void setup_timeout();
	void timeout();
	void handle_add_tcp_user(boost::uuids::uuid arg_uuid);
};
