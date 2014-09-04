#pragma once
#include "def.h"

struct userdata
{
	userdata(boost::uuids::uuid arg_uuid, ptr_user_session arg_user_session, udp::endpoint arg_endpoint)
		: m_uuid(arg_uuid)
		, m_user_session(arg_user_session)
		, m_endpoint(arg_endpoint)
	{
	}
	userdata(boost::uuids::uuid arg_uuid, ptr_user_session arg_user_session)
		: m_uuid(arg_uuid)
		, m_user_session(arg_user_session)
	{
	}
	boost::uuids::uuid m_uuid;
	ptr_user_session m_user_session;
	udp::endpoint m_endpoint;
};


namespace multiindex_tag{

	struct id{};
	struct session{};
	struct endpoint{};
}

typedef multi_index_container < userdata
	,indexed_by<
	  ordered_unique<tag<multiindex_tag::id>, BOOST_MULTI_INDEX_MEMBER(userdata, boost::uuids::uuid, m_uuid)>
	, ordered_non_unique<tag<multiindex_tag::session>, BOOST_MULTI_INDEX_MEMBER(userdata, ptr_user_session, m_user_session)>
	, ordered_non_unique<tag<multiindex_tag::endpoint>, BOOST_MULTI_INDEX_MEMBER(userdata, udp::endpoint, m_endpoint) >>> userdata_set;


void set_endpoint(userdata& arg_userdata, udp::endpoint arg_endpoint);