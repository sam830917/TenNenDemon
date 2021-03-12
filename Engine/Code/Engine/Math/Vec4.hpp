#pragma once
#include "Engine/Math/Vec3.hpp"

class Vec4
{
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;

public:
	Vec4();
	Vec4( Vec3 copyFrom, float initialW );
	explicit Vec4( float initialX, float initialY, float initialZ, float initialW );

	// Accessors (const methods)
	const Vec3	GetXYZ() const;

	// Operators (const)
	bool		operator==( const Vec4& compare ) const;		// Vec4 == Vec4
	bool		operator!=( const Vec4& compare ) const;		// Vec4 != Vec4
	const Vec4	operator+( const Vec4& vecToAdd ) const;		// Vec4 + Vec4
	const Vec4	operator-( const Vec4& vecToSubtract ) const;	// Vec4 - Vec4
	const Vec4	operator*( float uniformScale ) const;			// Vec4 * float
	const Vec4	operator/( float inverseScale ) const;			// Vec4 / float

																// Operators (self-mutating / non-const)
	void		operator+=( const Vec4& vecToAdd );				// Vec4 += Vec4
	void		operator-=( const Vec4& vecToSubtract );		// Vec4 -= Vec4
	void		operator*=( const float uniformScale );			// Vec4 *= float
	void		operator/=( const float uniformDivisor );		// Vec4 /= float
	void		operator=( const Vec4& copyFrom );				// Vec4 = Vec4

	// Standalone "friend" functions that are conceptually, but not actually, part of Vec4::
	friend const Vec4 operator*( float uniformScale, const Vec4& vecToScale );	// float * Vec4

private:

};