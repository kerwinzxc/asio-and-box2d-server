#pragma once
#include "def.h"

class util_makeindex
{
	set < unsigned int > useindexset;
public:
	util_makeindex();
	~util_makeindex();

	unsigned int useindex(unsigned int arg_tag); // ��� -- tag ��ŭ ����� ����Ѵ�.
	void reuseindex(unsigned int arg_index); // ����
};