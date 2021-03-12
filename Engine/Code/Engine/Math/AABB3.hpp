#pragma once
#include "Engine/Math/Vec3.hpp"

struct AABB3
{
public:
	Vec3 mins;
	Vec3 maxs;

public:
	~AABB3()	= default;
	AABB3()     = default;
	AABB3( const AABB3& copyFrom );
	explicit AABB3( const Vec3& inialMins, const Vec3& inialMaxs );

	const Vec3  GetCenter() const;

	void		SetCenter( const Vec3& newCenter );
	void		Translate( const Vec3& translation );
};