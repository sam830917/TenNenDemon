#include "Engine/Math/Plane2.hpp"

bool Plane2::GetPointIsInFront( Vec2 point ) const
{
	float pointDistFromPlane = DotProduct2D( point, normal );
	float distanceFromPlane = pointDistFromPlane - distance;
	return distanceFromPlane > 0.0f;
}

float Plane2::GetSignedDistanceFromPlane( Vec2 point ) const
{
	float pointDistFromPlane = DotProduct2D( point, normal );
	float distanceFromPlane = pointDistFromPlane - distance;
	return distanceFromPlane;
}