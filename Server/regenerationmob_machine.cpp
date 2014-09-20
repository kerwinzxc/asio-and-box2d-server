#include "stdafx.h"
#include "regenerationmob_machine.h"
#include "query.h"
#include "dumbmob_machine.h"
#include "room.h"

regenerationmob_machine::regenerationmob_machine(ptr_b2world arg_world, weakptr_room arg_room, unsigned int arg_gameobjectindex, float arg_left, float arg_right, float arg_top, float arg_bottom, float arg_regenerationtime, unsigned int arg_maxmobcount)
	: m_world(arg_world)
	, m_gameobjectindex(arg_gameobjectindex)
	, m_left(arg_left)
	, m_right(arg_right)
	, m_top(arg_top)
	, m_bottom(arg_bottom)
	, m_regenerationtime(arg_regenerationtime)
	, m_maxmobcount(arg_maxmobcount)
	, m_room(arg_room)
{

}


regenerationmob_machine::~regenerationmob_machine()
{
}

bool regenerationmob_machine::CreateRandomMob(float arg_tick)
{
	if (m_regenerationtime > arg_tick)
		return false;

	auto curiter = m_setgameobject.begin();
	while (curiter != m_setgameobject.end())
	{
		auto ptr = curiter->lock();
		if (ptr == NULL)
		{
			curiter = m_setgameobject.erase(curiter);
		}
		else
		{
			curiter++;
		}
		
	}

	if (m_setgameobject.size() >= m_maxmobcount)
	{
		return false;
	}

	int count = round(m_top) - round(m_bottom) - 1;

	
	boost::random::uniform_real_distribution<> u(m_bottom, m_top);
	boost::random::uniform_int_distribution<> i(round(m_bottom)+1, count);
	
	
	float x = u(rng);
	float y = i(rng);	

	b2Vec2 point(x, y);
	b2Vec2 point1(x, m_bottom);
	

	raycast_mobregencallback ray;

	m_world->RayCast(&ray,point, point1);
	aabbquery_notingbody _callback;
	b2AABB aabb;
	aabb.lowerBound.Set(x - 0.5, m_bottom);
	aabb.upperBound.Set(x + 0.5, y);
	m_world->QueryAABB(&_callback, aabb);

	if (_callback.isHit == true)
	{
		if (ray.m_hit == true)
		{
			if (ray.m_tag.getoption(FixtureTag_Ground) == true)
			{
				auto ptr = m_room.lock();
				ray.m_point.y += 1.0f;
				m_setgameobject.insert(ptr->createdumbmob(ray.m_point));
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	
	return false; // create
}

sc::result regenerationmob_common::react(const evtick &arg_evt)
{
	m_addtick += arg_evt.m_tick;
	if (context<regenerationmob_machine>().CreateRandomMob(m_addtick) == true)
		m_addtick = 0.0f;

	return discard_event();
}
