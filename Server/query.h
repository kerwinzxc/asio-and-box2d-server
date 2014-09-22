#pragma once
#include "def.h"

class raycastcallback : public b2RayCastCallback
{
public :
};

class raycast_closestcallback : public raycastcallback
{
public:
	raycast_closestcallback()
	{
		m_hit = false;
	}

	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
	{
		b2Body* body = fixture->GetBody();
		fixturetag tag((unsigned long)fixture->GetUserData());
		if (tag.getoption(FixtureTag_Body) != true)
		{
			return -1.0f;
		}

		m_hit = true;
		m_body = body;
		m_point = point;
		m_normal = normal;

		// By returning the current fraction, we instruct the calling code to clip the ray and
		// continue the ray-cast to the next fixture. WARNING: do not assume that fixtures
		// are reported in order. However, by clipping, we can always get the closest fixture.
		return fraction;
	}

	bool m_hit;
	b2Body* m_body;	
	b2Vec2 m_point;
	b2Vec2 m_normal;
};

// This callback finds any hit. Polygon 0 is filtered. For this type of query we are usually
// just checking for obstruction, so the actual fixture and hit point are irrelevant. 
class raycast_anycallback : public raycastcallback
{
public:
	raycast_anycallback()
	{
		m_hit = false;
	}

	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
	{
		b2Body* body = fixture->GetBody();

		m_hit = true;
		m_body = body;
		m_point = point;
		m_normal = normal;

		// At this point we have a hit, so we know the ray is obstructed.
		// By returning 0, we instruct the calling code to terminate the ray-cast.
		return 0.0f;
	}

	bool m_hit;
	b2Body* m_body;
	b2Vec2 m_point;
	b2Vec2 m_normal;
};

// This ray cast collects multiple hits along the ray. Polygon 0 is filtered.
// The fixtures are not necessary reported in order, so we might not capture
// the closest fixture.
class raycast_multiplecallback : public raycastcallback
{
public:
	enum
	{
		e_maxCount = 3
	};

	raycast_multiplecallback()
	{
		m_count = 0;
	}

	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
	{
		b2Body* body = fixture->GetBody();

		b2Assert(m_count < e_maxCount);

		m_bodys[m_count] = body;
		m_points[m_count] = point;
		m_normals[m_count] = normal;
		++m_count;

		if (m_count == e_maxCount)
		{
			// At this point the buffer is full.
			// By returning 0, we instruct the calling code to terminate the ray-cast.
			return 0.0f;
		}

		// By returning 1, we instruct the caller to continue without clipping the ray.
		return 1.0f;
	}
	b2Body* m_bodys[e_maxCount];
	b2Vec2 m_points[e_maxCount];
	b2Vec2 m_normals[e_maxCount];
	int32 m_count;
};


class raycast_mobregencallback : public raycastcallback
{
public:
	raycast_mobregencallback()
	{
		m_hit = false;
	}

	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
	{
		b2Body* body = fixture->GetBody();
		fixturetag tag((unsigned long)fixture->GetUserData());

		m_tag = tag;

		m_hit = true;
		m_point = point;
		m_normal = normal;

		// By returning the current fraction, we instruct the calling code to clip the ray and
		// continue the ray-cast to the next fixture. WARNING: do not assume that fixtures
		// are reported in order. However, by clipping, we can always get the closest fixture.
		return fraction;
	}

	bool m_hit;
	fixturetag m_tag;
	b2Vec2 m_point;
	b2Vec2 m_normal;
};



class aabbquery_notingbody : public b2QueryCallback
{
public:
	aabbquery_notingbody()
	{
		isHit = false;
	}

	bool ReportFixture(b2Fixture* fixture)
	{
		fixturetag tag = fixturetag((unsigned long)fixture->GetUserData());

		fixture->GetAABB(NULL);
		if (tag.getoption(FixtureTag_Ground) == false && tag.getoption(FixtureTag_SightRader) == false) // 그라운드가 아닌걸 만낫을때.
		{
			isHit = false;
			return false; // 종료 시킨다.
		}
		else
		{
			isHit = true; // 그라운드를 만낫으면 플래그를변경 해주고 다시 탐색 바디를 만날경우 false , 그대로 그라운드일경우 hit!
		}
		// Continue the query.
		return true;
	}
	bool isHit;
};




class raycast_guidelinecallback : public raycastcallback
{
public:
	raycast_guidelinecallback()
	{
		m_hit = false;
	}

	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
	{
		b2Body* body = fixture->GetBody();
		fixturetag tag((unsigned long)fixture->GetUserData());
		if (tag.getoption(FixtureTag_GuideLine) != true)
		{
			return -1.0f;
		}

		m_hit = true;
		m_body = body;
		m_point = point;

		// By returning the current fraction, we instruct the calling code to clip the ray and
		// continue the ray-cast to the next fixture. WARNING: do not assume that fixtures
		// are reported in order. However, by clipping, we can always get the closest fixture.
		return fraction;
	}

	bool m_hit;
	b2Body* m_body;
	b2Vec2 m_point;
//	b2Vec2 m_normal;
};