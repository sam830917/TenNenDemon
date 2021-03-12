#pragma once
#include "Engine/Math/Vec2.hpp"

class Collider2D;

struct Manifold2
{
	Vec2 normal = Vec2::ZERO;
	float penetration = 0.f;
	Vec2 contactPointMin = Vec2::ZERO;
	Vec2 contactPointMax = Vec2::ZERO;

	Vec2 GetContactPoint() const { return (contactPointMin + contactPointMax) * 0.5f; }
	void SetContactPoint( const Vec2& point ) 
	{ 
		contactPointMin = point;
		contactPointMax = point;
	}
	void SetContactPoint( const Vec2& min, const Vec2& max )
	{
		contactPointMin = min;
		contactPointMax = max;
	}
};

struct Collision2D // PoD type - Plane Old Data
{
public:
	Collider2D* me;
	Collider2D* them;
	Manifold2 manifold;

public:
	Vec2 GetNormal() const { return manifold.normal; }
	float GetPenetration() const { return manifold.penetration; }
	Collision2D GetInverse() const;

};