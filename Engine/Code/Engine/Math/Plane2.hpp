#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"

class Plane2
{
	Vec2 normal;
	float distance;

public:
	// methods
	Plane2()
		: normal( 0, 1 )
		, distance( 0 ) {}

	Plane2( Vec2 n, Vec2 pointOnPlane )
		: normal( n )
		, distance( DotProduct2D( pointOnPlane, n ) ) {}


	bool GetPointIsInFront( Vec2 point ) const;

	float GetSignedDistanceFromPlane( Vec2 point ) const;

};