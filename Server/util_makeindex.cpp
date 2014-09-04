#include "stdafx.h"
#include "util_makeindex.h"


util_makeindex::util_makeindex()
{
}


util_makeindex::~util_makeindex()
{
}

unsigned int util_makeindex::useindex(unsigned int arg_tag)
{
	int _index = arg_tag;
	while (true)
	{
		if (useindexset.find(_index) == useindexset.end())
		{
			useindexset.insert(_index);
			return _index;
		}
		_index++;
	}
	
}

void util_makeindex::reuseindex(unsigned int arg_index)
{
	useindexset.erase(arg_index);
}
