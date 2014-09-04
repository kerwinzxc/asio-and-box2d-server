#pragma once
#include "def.h"

class util_makeindex
{
	set < unsigned int > useindexset;
public:
	util_makeindex();
	~util_makeindex();

	unsigned int useindex(unsigned int arg_tag); // 사용 -- tag 만큼 띄워서 사용한다.
	void reuseindex(unsigned int arg_index); // 재사용
};