#pragma once
#include "def.h"



class fsm
	:public boost::enable_shared_from_this<fsm>
{
	std::map<string,ptr_state_unit> m_stateunitmap;

	void* m_userdata; // room , game_user , user......

	ptr_state_unit m_curstateunit;
	
public:
	fsm();
	~fsm();

	void add(string arg_name, ptr_state_unit arg_state_unit)
	{
		m_stateunitmap.insert(std::map<string, ptr_state_unit>::value_type(arg_name, arg_state_unit));
	}
	void addrule(int arg_condition ,string arg_prestate, string arg_curstate) // 이전 스테이트 - > 이후 스테이트c
	{

	}
	void addevent(string arg_state, boost::function<void(string)> arg_handler) // 스테이트 변경시 
	{

	}

	void start(string arg_statename);

	void transition(string arg_statename);

	void update(float deltatime);

};
