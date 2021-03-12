#pragma once
#include "Engine/Math/Vec2.hpp"

//-----------------------------------------------------------------------------------------------
struct Vec3
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

	static const Vec3 ZERO;
	static const Vec3 ONE;

public:
	// Construction/Destruction
	Vec3() {}												// default constructor (do nothing)
	Vec3( const Vec2& copyFrom, float copyZ );
	explicit Vec3( float initialX, float initialY, float initialZ );		// explicit constructor (from x, y, z)
	explicit Vec3( float initialValue );

	// Accessors (const methods)
	float		GetLength() const;
	float		GetLengthXY() const;
	float		GetLengthSquared() const;
	float		GetLengthXYSquared() const;
	float		GetAngleAboutZRadians() const;
	float		GetAngleAboutZDegrees() const;
	const Vec3	GetRotatedAboutZRadians( float deltaRadians ) const;
	const Vec3	GetRotatedAboutZDegrees( float deltaDegrees ) const;
	const Vec3	GetClamped( float maxLength ) const;
	const Vec3	GetNormalized() const;
	const Vec2	GetXY() const;

	void SetFromText( const char* text );

	// Operators (const)
	bool		operator==( const Vec3& compare ) const;		// Vec3 == Vec3
	bool		operator!=( const Vec3& compare ) const;		// Vec3 != Vec3
	const Vec3	operator+( const Vec3& vecToAdd ) const;		// Vec3 + Vec3
	const Vec3	operator-( const Vec3& vecToSubtract ) const;	// Vec3 - Vec3
	const Vec3	operator-() const;								// -vec3, i.e. "unary negation"
	const Vec3	operator*( float uniformScale ) const;			// Vec3 * float
	const Vec3	operator*( const Vec3& vecToMultiply ) const;
	const Vec3	operator/( float inverseScale ) const;			// Vec3 / float
	const Vec3	operator/( const Vec3& vecToDivide ) const;
																// Operators (self-mutating / non-const)
	void		operator+=( const Vec3& vecToAdd );				// Vec3 += Vec3
	void		operator-=( const Vec3& vecToSubtract );		// Vec3 -= Vec3
	void		operator*=( const float uniformScale );			// Vec3 *= float
	void		operator/=( const float uniformDivisor );		// Vec3 /= float
	void		operator=( const Vec3& copyFrom );				// Vec3 = Vec3

	// Standalone "friend" functions that are conceptually, but not actually, part of Vec3::
	friend const Vec3 operator*( float uniformScale, const Vec3& vecToScale );	// float * Vec3
};


