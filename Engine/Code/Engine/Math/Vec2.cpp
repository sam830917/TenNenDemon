#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>
#include "Engine/Core/StringUtils.hpp"

const Vec2 Vec2::ZERO = Vec2( 0.f, 0.f);
const Vec2 Vec2::ONE = Vec2( 1.f, 1.f );

//-----------------------------------------------------------------------------------------------
Vec2::Vec2( const Vec2& copy )
	: x( copy.x )
	, y( copy.y )
{
}

//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x( initialX )
	, y( initialY )
{
}

const Vec2 Vec2::MakeFromPolarRadians( float directionRadians, float length )
{
	return Vec2( length * CosDegrees( ConvertRadiansToDegrees(directionRadians) ), 
			     length * SinDegrees( ConvertRadiansToDegrees(directionRadians) ) );
}

const Vec2 Vec2::MakeFromPolarDegrees( float directionDegrees, float length )
{
	return Vec2( length * CosDegrees( directionDegrees ) ,
				 length * SinDegrees( directionDegrees ) );
}

float Vec2::GetLength() const
{
	return GetDistance2D( Vec2( x, y ), Vec2() );
}

float Vec2::GetLengthSquared() const
{
	return GetDistanceSquared2D( Vec2( x, y ), Vec2() );
}

float Vec2::GetAngleRadians() const
{
	return ConvertDegreesToRadians( Atan2Degrees( y, x ) );
}

float Vec2::GetAngleDegrees() const
{
	return Atan2Degrees( y, x );
}

const Vec2 Vec2::GetRotated90Degrees() const
{
	//return TransformPosition2D( Vec2( x, y ), 1.f, 90.f, Vec2() );
	return Vec2( -y, x );
}

const Vec2 Vec2::GetRotatedMinus90Degrees() const
{
	//return TransformPosition2D( Vec2( x, y ), 1.f, -90.f, Vec2() );
	return Vec2( y, -x );
}

const Vec2 Vec2::GetRotatedRadians( float deltaRadians ) const
{
	return TransformPosition2D( Vec2( x, y ), 1.f, ConvertRadiansToDegrees( deltaRadians ), Vec2() );
}

const Vec2 Vec2::GetRotatedDegrees( float deltaDegrees ) const
{
	return TransformPosition2D( Vec2( x, y ), 1.f, deltaDegrees, Vec2() );
}

const Vec2 Vec2::GetClamped( float maxLength ) const
{
	const float& distance = GetDistance2D( Vec2( x, y ), Vec2() );

	if ( distance < maxLength )
	{
		return Vec2( x, y );
	}
	else
	{
		return TransformPosition2D( Vec2( x, y ), maxLength / distance, 0.f, Vec2() );
	}
}

const Vec2 Vec2::GetNormalized() const
{
	const float& distance = GetDistance2D( Vec2( x, y ), Vec2() );

	return TransformPosition2D( Vec2( x, y ), 1.f / distance, 0.f, Vec2() );
}

const Vec2 Vec2::GetReflected( const Vec2& referVec2 ) const
{
	Vec2 velN = GetProjectedOnto2D( Vec2( x, y ), referVec2.GetNormalized() );
	Vec2 velT = Vec2( x, y ) - velN;
	return velT - velN;
}

void Vec2::SetAngleRadians( float newAngleRadians )
{
	const float& length = GetLength();
	const Vec2& newVec2 = MakeFromPolarRadians(newAngleRadians, length);

	x = newVec2.x;
	y = newVec2.y;
}

void Vec2::SetAngleDegrees( float newAngleDegrees )
{
	const float& length = GetLength();
	const Vec2& newVec2 = MakeFromPolarDegrees( newAngleDegrees, length );

	x = newVec2.x;
	y = newVec2.y;
}

void Vec2::SetPolarRadians( float newAngleRadians, float newLength )
{
	const Vec2& newVec2 = MakeFromPolarRadians( newAngleRadians, newLength );

	x = newVec2.x;
	y = newVec2.y;
}

void Vec2::SetPolarDegrees( float newAngleDegrees, float newLength )
{
	const Vec2& newVec2 = MakeFromPolarDegrees( newAngleDegrees, newLength );

	x = newVec2.x;
	y = newVec2.y;
}

void Vec2::Rotate90Degrees()
{
	float tempX = x;
	x = -y;
	y = tempX;
}

void Vec2::RotateMinus90Degrees()
{
	float tempX = x;
	x = y;
	y = -tempX;
}

void Vec2::RotateRadians( float deltaRadians )
{
	const Vec2& newVec2 = TransformPosition2D( Vec2( x, y ), 1.f, ConvertRadiansToDegrees( deltaRadians ), Vec2() );

	x = newVec2.x;
	y = newVec2.y;
}

void Vec2::RotateDegrees( float deltaDegrees )
{
	const Vec2& newVec2 = TransformPosition2D( Vec2( x, y ), 1.f, deltaDegrees, Vec2() );

	x = newVec2.x;
	y = newVec2.y;
}

void Vec2::SetLength( float newLength )
{
	const float& distance = GetLength();

	if ( distance != 0.f )
	{
		x *= newLength / distance;
		y *= newLength / distance;
	}
}

void Vec2::ClampLength( float maxLength )
{
	const float& distance = GetLength();

	if( distance > maxLength )
	{
		const Vec2& newVec2 = TransformPosition2D( Vec2( x, y ), maxLength / distance, 0.f, Vec2() );

		x = newVec2.x;
		y = newVec2.y;
	}
}

void Vec2::Normalize()
{
	const float& distance = GetLength();
	if ( distance != 0.f )
	{
		const Vec2& newVec2 = TransformPosition2D( Vec2( x, y ), 1.f / distance, 0.f, Vec2() );

		x = newVec2.x;
		y = newVec2.y;
	}
}

float Vec2::NormalizeAndGetPreviousLength()
{
	const float& previousLength = GetLength();
	Normalize();

	return previousLength;
}

void Vec2::Reflect( const Vec2& referVec2 )
{
	Vec2 velN = GetProjectedOnto2D( Vec2( x, y ), referVec2.GetNormalized() );
	Vec2 velT = Vec2( x, y ) - velN;
	Vec2 vel = velT - velN;
	x = vel.x;
	y = vel.y;
}

void Vec2::SetFromText( const char* text )
{
	Strings stringList = SplitStringOnDelimiter( text, ',' );
	if ( (int) stringList.size() == 2 )
	{
		float firstNum = (float) atof( stringList[0].c_str() );
		float secondNum = (float) atof( stringList[1].c_str() );
		x = firstNum;
		y = secondNum;
	}
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator + ( const Vec2& vecToAdd ) const
{
	return Vec2( x + vecToAdd.x, y + vecToAdd.y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-( const Vec2& vecToSubtract ) const
{
	return Vec2( x - vecToSubtract.x, y - vecToSubtract.y );
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-() const
{
	return Vec2( -x, -y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( float uniformScale ) const
{
	return Vec2( uniformScale * x, uniformScale * y );
}


//------------------------------------------------------------------------------------------------
//¡õ
const Vec2 Vec2::operator*( const Vec2& vecToMultiply ) const
{
	return Vec2( x * vecToMultiply.x, y * vecToMultiply.y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator/( float inverseScale ) const
{
	return Vec2( x / inverseScale, y / inverseScale );
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( const Vec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( const Vec2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( const Vec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
const Vec2 operator*( float uniformScale, const Vec2& vecToScale )
{
	return Vec2( vecToScale.x * uniformScale, vecToScale.y * uniformScale );
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( const Vec2& compare ) const
{
	return (x == compare.x && y == compare.y)?true:false;
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( const Vec2& compare ) const
{
	return (x != compare.x || y != compare.y)?true:false;
}

