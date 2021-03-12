#pragma once
#include "Engine/Math/Vec2.hpp"

struct LineSegment2
{
public:
	Vec2 m_start;
	Vec2 m_end;

public:
	LineSegment2() = default;
	explicit LineSegment2( const Vec2& start, const Vec2& end );

	// Accessors
	const Vec2	GetNearestPoint( const Vec2& referencePoint ) const;

	// Mutators
	void		Translate( const Vec2& translation );
	void		RotateDegrees( float relativeRotationDegrees );

	static bool ClipSegmentToSegment( const LineSegment2& toClipLine, const LineSegment2& refLine, LineSegment2& outLine );
};