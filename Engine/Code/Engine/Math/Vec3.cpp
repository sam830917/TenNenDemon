#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

const Vec3 Vec3::ZERO = Vec3( 0.f, 0.f, 0.f );
const Vec3 Vec3::ONE = Vec3( 1.f, 1.f, 1.f );

Vec3::Vec3( const Vec2& copyFrom, float copyZ )
	:x(copyFrom.x),
	y(copyFrom.y),
	z(copyZ)
{
}

//-----------------------------------------------------------------------------------------------
Vec3::Vec3( float initialX, float initialY, float initialZ )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
{
}

Vec3::Vec3( float initialValue )
	: x( initialValue )
	, y( initialValue )
	, z( initialValue )
{
}

float Vec3::GetLength() const
{
	return GetDistance3D( Vec3( x, y, z ), Vec3() );
}

float Vec3::GetLengthXY() const
{
	return GetDistance3D( Vec3( x, y, 0.f ), Vec3() );
}

float Vec3::GetLengthSquared() const
{
	return GetDistanceSquared3D( Vec3( x, y, z ), Vec3() );
}

float Vec3::GetLengthXYSquared() const
{
	return GetDistanceSquared3D( Vec3( x, y, 0.f ), Vec3() );
}

float Vec3::GetAngleAboutZRadians() const
{
	return ConvertDegreesToRadians( Atan2Degrees( y, x ) );
}

float Vec3::GetAngleAboutZDegrees() const
{
	return Atan2Degrees( y, x );
}

const Vec3 Vec3::GetRotatedAboutZRadians( float deltaRadians ) const
{
	return TransformPosition3DXY( Vec3( x, y, z ), 1.f, ConvertRadiansToDegrees( deltaRadians ), Vec2() );
}

const Vec3 Vec3::GetRotatedAboutZDegrees( float deltaDegrees ) const
{
	return TransformPosition3DXY( Vec3( x, y, z ), 1.f, deltaDegrees, Vec2() );
}

const Vec3 Vec3::GetClamped( float maxLength ) const
{
	const float& distance = GetDistance3D( Vec3( x, y, z ), Vec3() );

	if( distance < maxLength )
	{
		return Vec3( x, y, z );
	}
	else
	{
		return TransformPosition3DXY( Vec3( x, y, z ), maxLength / distance, 0.f, Vec2() );
	}
}

const Vec3 Vec3::GetNormalized() const
{
	const float& distance = GetDistance3D( Vec3( x, y, z ), Vec3() );

	return distance!= 0.f ? Vec3( x / distance, y / distance, z / distance ) : Vec3::ZERO;
}

const Vec2 Vec3::GetXY() const
{
	return Vec2( x, y );
}

void Vec3::SetFromText( const char* text )
{
	Strings stringList = SplitStringOnDelimiter( text, ',' );
	if( (int)stringList.size() == 3 )
	{
		float firstNum = (float)atof( stringList[0].c_str() );
		float secondNum = (float)atof( stringList[1].c_str() );
		float thirdNum = (float)atof( stringList[2].c_str() );
		x = firstNum;
		y = secondNum;
		z = thirdNum;
	}
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator + ( const Vec3& vecToAdd ) const
{
	return Vec3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z );
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-( const Vec3& vecToSubtract ) const
{
	return Vec3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z );
}

const Vec3 Vec3::operator-() const
{
	return Vec3( -x, -y, -z );
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*( float uniformScale ) const
{
	return Vec3( uniformScale * x, uniformScale * y, uniformScale * z );
}

const Vec3 Vec3::operator*( const Vec3& vecToMultiply ) const
{
	return Vec3( x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z );
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator/( float inverseScale ) const
{
	return Vec3( x / inverseScale, y / inverseScale, z / inverseScale );
}

const Vec3 Vec3::operator/( const Vec3& vecToDivide ) const
{
	return Vec3( x / vecToDivide.x, y / vecToDivide.y, z / vecToDivide.z );
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator+=( const Vec3& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator-=( const Vec3& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator=( const Vec3& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}


//-----------------------------------------------------------------------------------------------
const Vec3 operator*( float uniformScale, const Vec3& vecToScale )
{
	return Vec3( vecToScale.x * uniformScale, vecToScale.y * uniformScale, vecToScale.z * uniformScale );
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator==( const Vec3& compare ) const
{
	return (x == compare.x && y == compare.y && z == compare.z) ? true : false;
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator!=( const Vec3& compare ) const
{
	return (x != compare.x || y != compare.y || z != compare.z) ? true : false;
}

