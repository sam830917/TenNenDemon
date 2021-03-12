#include "IntVec2.hpp"
#include "Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>
#include "Engine/Core/StringUtils.hpp"

const IntVec2 IntVec2::ZERO = IntVec2( 0, 0 );
const IntVec2 IntVec2::ONE = IntVec2( 1, 1 );

IntVec2::IntVec2( const IntVec2& copyFrom )
	: x( copyFrom.x )
	, y( copyFrom.y )
{

}

IntVec2::IntVec2( int initialX, int initialY )
	: x( initialX )
	, y( initialY )
{

}

float IntVec2::GetLength() const
{
	return GetDistance2D( Vec2( static_cast<float>(x), static_cast<float>(y) ), Vec2() );
}

int IntVec2::GetLengthSquared() const
{
	return (int) GetDistanceSquared2D( Vec2( static_cast<float>(x), static_cast<float>(y) ), Vec2() );
}

int IntVec2::GetTaxicabLength() const
{
	int iniX = (x < 0) ? -x : x;
	int iniY = (y < 0) ? -y : y;

	return iniX + iniY;
}

float IntVec2::GetOrientationRadians() const
{
	return ConvertDegreesToRadians( Atan2Degrees( static_cast<float>(y), static_cast<float>(x) ) );;
}

float IntVec2::GetOrientationDegrees() const
{
	return Atan2Degrees( (float) y, (float) x );
}

const IntVec2 IntVec2::GetRotated90Degrees() const
{
	return IntVec2( -y, x );
}

const IntVec2 IntVec2::GetRotatedMinus90Degrees() const
{
	return IntVec2( y, -x );
}

void IntVec2::Rotate90Degrees()
{
	int temp = x;
	x = -y;
	y = temp;
}

void IntVec2::RotateMinus90Degrees()
{
	int temp = x;
	x = y;
	y = -temp;
}

void IntVec2::SetFromText( const char* text )
{
	Strings stringList = SplitStringOnDelimiter( text, ',' );
	if( (int) stringList.size() == 2 )
	{
		int firstNum = atoi( stringList[0].c_str() );
		int secondNum = atoi( stringList[1].c_str() );
		x = firstNum;
		y = secondNum;
	}
}

bool IntVec2::operator==( const IntVec2& compare ) const
{
	return (x == compare.x && y == compare.y)?true:false;
}

bool IntVec2::operator!=( const IntVec2& compare ) const
{
	return (x != compare.x || y != compare.y)?true:false;
}

const IntVec2 IntVec2::operator+( const IntVec2& vecToAdd ) const
{
	return IntVec2( x + vecToAdd.x, y + vecToAdd.y );
}

const IntVec2 IntVec2::operator-( const IntVec2& vecToSubtract ) const
{
	return IntVec2( x - vecToSubtract.x, y - vecToSubtract.y );
}

const IntVec2 IntVec2::operator-() const
{
	return IntVec2( -x, -y );
}

const IntVec2 IntVec2::operator*( int uniformScale ) const
{
	return IntVec2( uniformScale * x, uniformScale * y );
}

const IntVec2 IntVec2::operator*( const IntVec2& vecToMultiply ) const
{
	return IntVec2( x * vecToMultiply.x, y * vecToMultiply.y );
}

const IntVec2 IntVec2::operator/( float inverseScale ) const
{
	return IntVec2( static_cast<int>(x / inverseScale), static_cast<int>(y / inverseScale) );
}

void IntVec2::operator+=( const IntVec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}

void IntVec2::operator-=( const IntVec2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}

void IntVec2::operator*=( const int uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}

void IntVec2::operator/=( const int uniformDivisor )
{
	x = static_cast<int>(x / uniformDivisor);
	y = static_cast<int>(y / uniformDivisor);
}

void IntVec2::operator=( const IntVec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}

const IntVec2 operator*( int uniformScale, const IntVec2& vecToScale )
{
	return IntVec2( vecToScale.x * uniformScale, vecToScale.y * uniformScale );
}
