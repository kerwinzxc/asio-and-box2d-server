#include "stdafx.h"
#include "interface.h"

#include "util_makeindex.h"



gameobject::gameobject(ptr_makeindex arg_makeindex)
	:m_makeindex(arg_makeindex)
{
	m_gameobjectindex = m_makeindex->useindex(m_gameobjectindex);
}

gameobject::~gameobject()
{
	m_makeindex->reuseindex(m_gameobjectindex);
}
