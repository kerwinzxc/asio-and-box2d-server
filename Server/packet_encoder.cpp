#include "stdafx.h"
#include "packet_encoder.h"
#include "body.pb.h"
#include "packet_data.h"

packet_encoder::packet_encoder(ptr_packet_data arg_packet)
	: m_packetdata(arg_packet)
	, m_bodyarray(NULL)
	, m_bodycoded(NULL)
	, m_headerarray(NULL)
	, m_headercoded(NULL)
{

	m_bodyarray = new google::protobuf::io::ArrayOutputStream(arg_packet->get_body(), arg_packet->get_maxbodysize());
	m_bodycoded = new google::protobuf::io::CodedOutputStream(m_bodyarray);

	m_headerarray = new google::protobuf::io::ArrayOutputStream(arg_packet->get_header(), arg_packet->get_headersize());
	m_headercoded = new google::protobuf::io::CodedOutputStream(m_headerarray);
}


packet_encoder::~packet_encoder()
{
	delete m_bodycoded;
	delete m_bodyarray;	
	delete m_headercoded;
	delete m_headerarray;
	
}

void packet_encoder::addmessage(google::protobuf::Message* arg_message)
{
	if (arg_message->ByteSize() == 0) return;

	
	
	BYTE messagetype = arg_message->GetDescriptor()->index();
	
	m_bodycoded->WriteRaw(&messagetype, 1); // write messagetype

	int size = arg_message->ByteSize();
	m_bodycoded->WriteRaw(&size,4); // write messagesize
	arg_message->SerializeToCodedStream(m_bodycoded);

}

void packet_encoder::makeheader()
{
	if (m_bodycoded->ByteCount() == 0) return;
	int size = m_bodycoded->ByteCount();
	m_headercoded->WriteRaw(&size,4);
		
	m_packetdata->set_bodysize(size);

}
