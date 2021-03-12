#pragma once
#include "Engine/Math/Vec2.hpp"
#include <string>

struct Capsule2
{
public:
	Vec2	m_boneStart;
	Vec2	m_boneEnd;
	float	m_radius;

public:
	Capsule2() = default;
	explicit Capsule2( const Vec2& boneStart, const Vec2& boneEnd, float radius );

	// Accessors
	bool		IsPointInside( const Vec2& point ) const;
	const Vec2	GetNearestPoint( const Vec2& referencePoint ) const;

	// Mutators
	void		Translate( const Vec2& translation );
	void		RotateDegrees( float relativeRotationDegrees );
};