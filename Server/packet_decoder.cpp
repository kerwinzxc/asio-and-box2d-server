#include "stdafx.h"
#include "packet_decoder.h"

#include "packet_data.h"

packet_decoder::packet_decoder(ptr_packet_data arg_data)
	:m_packetdata(arg_data)
{
	
}


packet_decoder::~packet_decoder()
{


}

unsigned int packet_decoder::getbodysize()
{	
	return m_bodysize;
}

void packet_decoder::decode_header()
{
	m_headerarray = new google::protobuf::io::ArrayInputStream(m_packetdata->get_header(), m_packetdata->get_headersize());
	m_headercoded = new google::protobuf::io::CodedInputStream(m_headerarray);
	
	m_headercoded->ReadRaw(&m_bodysize,4);
}

void packet_decoder::decode_body()
{
	m_bodyarray = new google::protobuf::io::ArrayInputStream(m_packetdata->get_body(), getbodysize());
	m_bodycoded = new google::protobuf::io::CodedInputStream(m_bodyarray);
}

BYTE packet_decoder::getbody(ptr_proto_message& arg_protomessage)
{
	if(m_bodycoded->CurrentPosition() == m_bodysize)
		return 255;
	
	BYTE _type;
	int _size;
	m_bodycoded->ReadRaw(&_type, 1);
	m_bodycoded->ReadRaw(&_size,4);
	
	google::protobuf::Message* _message = NULL;
	
	if (_type == databody::gameuser_data::descriptor()->index())
	{
		databody::gameuser_data* d = new databody::gameuser_data();
		d->ParseFromCodedStream(m_bodycoded);		
		_message = (google::protobuf::Message*)d;
	}
	if (_type == databody::gameuser_info::descriptor()->index())
	{
		databody::gameuser_info* d = new databody::gameuser_info();
		d->ParseFromCodedStream(m_bodycoded);
		_message = (google::protobuf::Message*)d;

	}
	if (_type == databody::staticobject_info::descriptor()->index())
	{
		databody::staticobject_info* d = new databody::staticobject_info();
		d->ParseFromCodedStream(m_bodycoded);
		_message = (google::protobuf::Message*)d;
	}
	if (_type == databody::login::descriptor()->index())
	{
		databody::login* d = new databody::login();
		d->ParseFromCodedStream(m_bodycoded);
		_message = (google::protobuf::Message*)d;
	}
	if (_type == databody::join::descriptor()->index())
	{
		databody::join* d = new databody::join();
		d->ParseFromCodedStream(m_bodycoded);
		_message = (google::protobuf::Message*)d;
	}
	if (_type == databody::exit::descriptor()->index())
	{
		databody::exit* d = new databody::exit();
		d->ParseFromCodedStream(m_bodycoded);
		_message = (google::protobuf::Message*)d;
	}
	if (_type == databody::move::descriptor()->index())
	{
		databody::move* d = new databody::move();
		d->ParseFromCodedStream(m_bodycoded);
		_message = (google::protobuf::Message*)d;
	}
	if (_type == databody::jump::descriptor()->index())
	{
		databody::jump* d = new databody::jump();
		d->ParseFromCodedStream(m_bodycoded);
		_message = (google::protobuf::Message*)d;
	}
	if (_type == databody::skill1::descriptor()->index())
	{
		databody::skill1* d = new databody::skill1();
		d->ParseFromCodedStream(m_bodycoded);
		_message = (google::protobuf::Message*)d;
	}
	if (_type == databody::commandresult::descriptor()->index())
	{
		databody::commandresult* d = new databody::commandresult();
		d->ParseFromCodedStream(m_bodycoded);
		_message = (google::protobuf::Message*)d;
	}

	arg_protomessage = ptr_proto_message(_message);

	return _type;
}
