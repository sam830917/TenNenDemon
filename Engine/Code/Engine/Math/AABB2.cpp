#include "AABB2.hpp"
#include "MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <math.h>

const AABB2 AABB2::ZERO_TO_ONE = AABB2( Vec2(), Vec2( 1.f, 1.f ) );

AABB2::AABB2( const AABB2& copyFrom )
	:mins( copyFrom.mins ),
	maxs( copyFrom.maxs )
{
}

AABB2::AABB2( const Vec2& inialMins, const Vec2& inialMaxs )
	: mins( inialMins ),
	maxs( inialMaxs )
{
}

AABB2::AABB2( float minX, float minY, float maxX, float maxY )
	: mins( Vec2( minX, minY ) ),
	maxs( Vec2( maxX, maxY ) )
{
}

bool AABB2::IsPointInside( const Vec2& point ) const
{
	if ( mins.x <= point.x && mins.y <= point.y && maxs.x >= point.x && maxs.y >= point.y )
	{
		return true;
	}
	return false;
}

const Vec2 AABB2::GetCenter() const
{
	return Vec2( ( mins.x + maxs.x ) * 0.5f, ( mins.y + maxs.y ) * 0.5f );
}

const Vec2 AABB2::GetDimensions() const
{
	return maxs - mins;
}

const Vec2 AABB2::GetNearestPoint( const Vec2& referencePosition ) const
{
	float x = referencePosition.x;
	float y = referencePosition.y;

	if ( referencePosition.x < mins.x )
	{
		x = mins.x;
	}
	else if ( referencePosition.x > maxs.x )
	{
		x = maxs.x;
	}
	if ( referencePosition.y < mins.y )
	{
		y = mins.y;
	}
	else if ( referencePosition.y > maxs.y )
	{
		y = maxs.y;
	}

	return Vec2( x, y );
}

const Vec2 AABB2::GetPointAtUV( const Vec2& uvCoordsZeroToOne ) const
{
	float x = RangeMap( 0.f, 1.f, mins.x, maxs.x, uvCoordsZeroToOne.x );
	float y = RangeMap( 0.f, 1.f, mins.y, maxs.y, uvCoordsZeroToOne.y );

	return Vec2( x, y );
}

const Vec2 AABB2::GetUVForPoint( const Vec2& point ) const
{
	float x = RangeMap( mins.x, maxs.x, 0.f, 1.f, point.x );
	float y = RangeMap( mins.y, maxs.y, 0.f, 1.f, point.y );

	return Vec2( x, y );
}

void AABB2::Translate( const Vec2& translation )
{
	mins += translation;
	maxs += translation;
}

void AABB2::SetCenter( const Vec2& newCenter )
{
	Vec2 oldCenter = GetCenter();
	Translate( newCenter - oldCenter );
}

void AABB2::SetDimensions( const Vec2& newDimesions )
{
	Vec2 oldCenter = GetCenter();
	mins = Vec2();
	maxs = newDimesions;
	SetCenter(oldCenter);
}

void AABB2::StretchToIncludePoint( const Vec2& point )
{
	if ( !IsPointInside( point ) )
	{
		// Max
		if ( maxs.x < point.x )
		{
			maxs.x = point.x;
		}
		if ( maxs.y < point.y )
		{
			maxs.y = point.y;
		}
		// Min
		if( mins.x > point.x )
		{
			mins.x = point.x;
		}
		if( mins.y > point.y )
		{
			mins.y = point.y;
		}
	}
}

void AABB2::FitWithinBounds( const AABB2& bounds )
{
	if ( mins.x < bounds.mins.x )
	{
		Translate( Vec2( bounds.mins.x - mins.x, 0.f ) );
	}
	if ( maxs.x > bounds.maxs.x )
	{
		Translate( Vec2( bounds.maxs.x - maxs.x, 0.f ) );
	}
	if( mins.y < bounds.mins.y )
	{
		Translate( Vec2( 0.f, bounds.mins.y - mins.y ) );
	}
	if( maxs.y > bounds.maxs.y )
	{
		Translate( Vec2( 0.f, bounds.maxs.y - maxs.y ) );
	}
}

void AABB2::SetFromText( const char* text )
{
	Strings stringList = SplitStringOnDelimiter( text, ',' );
	if( stringList.size() == 4 )
	{
		maxs = Vec2( (float)atof( stringList[2].c_str() ), (float)atof( stringList[3].c_str() ) );
		mins = Vec2( (float)atof( stringList[0].c_str() ), (float)atof( stringList[1].c_str() ) );
	}
}

AABB2 AABB2::CarveBoxOffRight( float fractionOfWidth, float additionalWidth )
{
	float width = maxs.x - mins.x;
	float carveWidth = (fractionOfWidth * width) + additionalWidth;
	AABB2 carvePiece( maxs.x - carveWidth, mins.y, maxs.x, maxs.y );
	maxs.x = carvePiece.mins.x;
	return carvePiece;
}

AABB2 AABB2::CarveBoxOffLeft( float fractionOfWidth, float additionalWidth )
{
	float width = maxs.x - mins.x;
	float carveWidth = (fractionOfWidth * width) + additionalWidth;
	AABB2 carvePiece( mins.x, mins.y, mins.x + carveWidth, maxs.y );
	mins.x = carvePiece.maxs.x;
	return carvePiece;
}

AABB2 AABB2::CarveBoxOffTop( float fractionOfHeight, float additionalHeight )
{
	float height = maxs.y - mins.y;
	float carveHeight = (fractionOfHeight * height) + additionalHeight;
	AABB2 carvePiece( mins.x, maxs.y - carveHeight, maxs.x, maxs.y );
	maxs.y = carvePiece.mins.y;
	return carvePiece;
}

AABB2 AABB2::CarveBoxOffBottom( float fractionOfHeight, float additionalHeight )
{
	float height = maxs.y - mins.y;
	float carveHeight = (fractionOfHeight * height) + additionalHeight;
	AABB2 carvePiece( mins.x, mins.y, maxs.x, mins.y + carveHeight );
	mins.y = carvePiece.maxs.y;
	return carvePiece;
}

AABB2 AABB2::GetTranslateAtPivot( const AABB2& bounds, Vec4 rationOffsetPosition, Vec2 pivot )
{
	AABB2 newBounds( mins, maxs );
	newBounds.TranslateAtPivot( bounds, rationOffsetPosition, pivot );
	return newBounds;
}

void AABB2::TranslateAtPivot( const AABB2& bounds, Vec4 rationOffsetPosition, Vec2 pivot )
{
	Vec2 dimension = GetDimensions();
	Vec2 position = -dimension * pivot;
	Vec2 rationPosition = Vec2( rationOffsetPosition.x, rationOffsetPosition.y );
	Vec2 offsetPosition = Vec2( rationOffsetPosition.z, rationOffsetPosition.w );

	mins = position + (rationPosition * bounds.GetDimensions()) + offsetPosition;
	maxs = mins + dimension;
}

bool AABB2::operator==( const AABB2& compareWith ) const
{
	return mins==compareWith.mins && maxs==compareWith.maxs;
}

void AABB2::operator=( const AABB2& assignFrom )
{
	mins = assignFrom.mins;
	maxs = assignFrom.maxs;
}
