#include "LineSegment2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/FloatRange.hpp"

LineSegment2::LineSegment2( const Vec2& start, const Vec2& end )
	:m_start( start ),
	m_end( end )
{
}

const Vec2 LineSegment2::GetNearestPoint( const Vec2& referencePoint ) const
{
	Vec2 lineDisplacement = m_end - m_start;
	Vec2 pointAndStartDisplacement = referencePoint - m_start;
	Vec2 pointAndEndDisplacement = referencePoint - m_end;
	float pointAndStartdot = DotProduct2D( lineDisplacement, pointAndStartDisplacement );
	float pointAndEnddot = DotProduct2D( lineDisplacement, pointAndEndDisplacement );
	// Point on start position side
	if( pointAndStartdot < 0.f )
	{
		return m_start;
	}
	// Point on end position side
	else if( pointAndEnddot > 0.f )
	{
		return m_end;
	}
	// Point on mid side
	else
	{
		Vec2 bonePoint = GetProjectedOnto2D( pointAndStartDisplacement, lineDisplacement ) + m_start;
		return bonePoint;
	}
}

void LineSegment2::Translate( const Vec2& translation )
{
	m_start += translation;
	m_end += translation;
}

void LineSegment2::RotateDegrees( float relativeRotationDegrees )
{
	Vec2 lineDisplacement = m_end - m_start;
	Vec2 center = m_start + lineDisplacement * 0.5;
	Vec2 centerToEndDis = m_end - center;
	Vec2 centerToStartDis = m_start - center;
	centerToStartDis.RotateDegrees( relativeRotationDegrees );
	centerToEndDis.RotateDegrees( relativeRotationDegrees );

	m_start = center + centerToStartDis;
	m_end = center + centerToEndDis;
}

bool LineSegment2::ClipSegmentToSegment( const LineSegment2& toClipLine, const LineSegment2& refLine, LineSegment2& outLine )
{
	Vec2 rd = refLine.m_end - refLine.m_start;
	rd.Normalize();
	Vec2 refScaleLine = Vec2( DotProduct2D( refLine.m_start, rd ), DotProduct2D( refLine.m_end, rd ) );
	Vec2 toClipScaleLine = Vec2( DotProduct2D( toClipLine.m_start, rd ), DotProduct2D( toClipLine.m_end, rd ) );

	FloatRange clippedLine = FloatRange( GetMax( refScaleLine.x, toClipScaleLine.x ), GetMin( refScaleLine.y, toClipScaleLine.y ) );

	if( clippedLine.minimum > clippedLine.maximum )
	{
		return false;
	}
	outLine.m_start = RangeMap( toClipScaleLine.x, toClipScaleLine.y, toClipLine.m_start, toClipLine.m_end, clippedLine.minimum );
	outLine.m_end = RangeMap( toClipScaleLine.x, toClipScaleLine.y, toClipLine.m_start, toClipLine.m_end, clippedLine.maximum );

	return true;
}
