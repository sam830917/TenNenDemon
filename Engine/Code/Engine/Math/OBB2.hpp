#pragma once
#include "Vec2.hpp"
#include "AABB2.hpp"

struct OBB2
{
	Vec2	m_center;
	Vec2	m_halfDimensions;				// we store (halfWidth, halfHeight), so (width, height) = 2.f * m_halfDimensions
	Vec2	m_iBasisNormal = Vec2( 1.f, 0.f );	// oriented i-basis normal; j-basis is +90 degrees ( -iBasis.y, iBasis.x )

	// Construction / destruction
	OBB2() = default;
	~OBB2() = default;
	OBB2( const OBB2& copyFrom );
	explicit OBB2( const Vec2& center, const Vec2& fullDimensions, const Vec2& iBasisNormal=Vec2( 1.f, 0.f ) );
	explicit OBB2( const Vec2& center, const Vec2& fullDimensions, float orientationDegrees = 0.f );
	explicit OBB2( const AABB2& asAxisAlignedBox, float orientationDegrees = 0.f );

	// Accessors (const methods)
	bool		IsPointInside( const Vec2& point ) const;
	const Vec2	GetCenter() const;
	const Vec2	GetDimensions() const;
	const Vec2	GetIBasisNormal() const;
	const Vec2	GetJBasisNormal() const;
	float		GetOrientationDegrees() const;
	const Vec2	GetNearestPoint( const Vec2& referencePos ) const;		// finds nearest point on/in box to reference position
	const Vec2	GetPointAtUV( const Vec2& uvCoordsZeroToOne ) const;	// pass (0,0) for relative mins, (.5,.5) for center, (1,1) for relative maxs
	const Vec2	GetUVForPoint( const Vec2& point ) const;				// returns (0,0) for relative mins, (.5,.5) for center, (1,1) for relative maxs
	float		GetOuterRadius() const;									// radius of circle entirely containing the OBB (= w/2 + h/2)
	float		GetInnerRadius() const;									// radius of circle entirely within the OBB (tightly)
	void		GetCornerPosition( Vec2* out_fourPoints ) const;		// place 4 corner positions into this float[4] array

	// Mutators (non-const methods)
	void Translate( const Vec2& translation );
	void SetCenter( const Vec2& newCenter );
	void SetDimensions( const Vec2& newDimensions );
	void SetOrientationDegrees( float completelyNewAbsoluteOrientation );
	void RotateByDegrees( float relativeRotationDegrees );
	void StretchToIncludePoint( const Vec2& point );	// does not stretch if point is already on or in box
	void Fix(); // "corrects" the box: (1) abs() halfDimensions, and (2) normalize iBasis (or restore to 1,0 if currently 0,0)

	// Operators (self-mutating / non-const)
	void operator=( const OBB2& assignFrom );

};