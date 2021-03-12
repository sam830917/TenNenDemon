#pragma once
#include "Vec2.hpp"
#include "Vec4.hpp"

struct AABB2
{
public:
	Vec2 mins;
	Vec2 maxs;
	static const AABB2 ZERO_TO_ONE;

public:
	~AABB2()	= default;
	AABB2()     = default;
	AABB2( const AABB2& copyFrom );  // copy constructor ( from another ivec2 )
	explicit AABB2( const Vec2& mins, const Vec2& maxs );  // explicit constructor (from minS, maxS )
	explicit AABB2( float minX, float minY, float maxX, float maxY );  //explicit constructor ( from x1,y1,x2,y2)
	bool        IsPointInside( const Vec2& point ) const;
	const Vec2  GetCenter() const;
	const Vec2  GetDimensions() const;
	const Vec2  GetNearestPoint( const Vec2& referencePosition ) const;
	const Vec2  GetPointAtUV( const Vec2& uvCoordsZeroToOne ) const;
	const Vec2  GetUVForPoint( const Vec2& point ) const;
	AABB2		GetBoxWithIn( const Vec2& dimensions, const Vec2& alignment ) const;
	AABB2		GetBoxAtLeft( float fractionOfWidth, float additionalWidth = 0.f ) const;
	AABB2		GetBoxAtRight( float fractionOfWidth, float additionalWidth = 0.f ) const;
	AABB2		GetBoxAtBottom( float fractionOfHeight, float additionalHeight = 0.f ) const;
	AABB2		GetBoxAtTop( float fractionOfHeight, float additionalHeight = 0.f ) const;
	float		GetOuterRadius() const;
	float		GetInnerRadius() const;
	void		GetCornerPositions( Vec2* out_fourPoints ) const;

	void		Translate( const Vec2& translation );
	void		SetCenter( const Vec2& newCenter );
	void		SetDimensions( const Vec2& newDimesions );
	void		StretchToIncludePoint( const Vec2& point ); // does not stretch if the point is already on or in the box
	void		FitWithinBounds( const AABB2& bounds );
	AABB2		CarveBoxOffRight( float fractionOfWidth, float additionalWidth = 0.f );
	AABB2		CarveBoxOffLeft( float fractionOfWidth, float additionalWidth = 0.f );
	AABB2		CarveBoxOffTop( float fractionOfHeight, float additionalHeight = 0.f );
	AABB2		CarveBoxOffBottom( float fractionOfHeight, float additionalHeight = 0.f );
	AABB2		GetTranslateAtPivot( const AABB2& bounds, Vec4 rationOffsetPosition, Vec2 pivot );
	void		TranslateAtPivot( const AABB2& bounds, Vec4 rationOffsetPosition, Vec2 pivot );

	void		SetFromText( const char* text );

	bool		operator==( const AABB2& compareWith ) const;
	void		operator=( const AABB2& assignFrom );

};