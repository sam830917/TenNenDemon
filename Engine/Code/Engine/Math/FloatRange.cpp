#include "FloatRange.hpp"
#include "RandomNumberGenerator.hpp"
#include <math.h>
#include "Engine/Core/StringUtils.hpp"

FloatRange::FloatRange( float minAndMax )
{
	minimum = minAndMax;
	maximum = minAndMax;
}

FloatRange::FloatRange( float min, float max )
{
	minimum = min;
	maximum = max;
}

FloatRange::FloatRange( const char* asText )
{
	Strings stringList = SplitStringOnDelimiter( asText, '~' );
	if( (int) stringList.size() == 1 )
	{
		float num = (float) atof( stringList[0].c_str() );
		minimum = num;
		maximum = num;
	}
	if( (int) stringList.size() == 2 )
	{
		float firstNum = (float) atof( stringList[0].c_str() );
		float secondNum = (float) atof( stringList[1].c_str() );
		minimum = firstNum;
		maximum = secondNum;
	}
}

bool FloatRange::IsInRange( float value ) const
{
	if( minimum <= value && maximum >= value )
	{
		return true;
	}
	return false;
}

bool FloatRange::DoesOverlap( const FloatRange& otherRange ) const
{
	if( IsInRange( otherRange.minimum ) || IsInRange( maximum ) )
	{
		return true;
	}
	return false;
}

std::string FloatRange::GetAsString() const
{
	if( maximum == minimum )
	{
		return std::to_string( minimum );
	}
	else
	{
		return Stringf( "%f~%f", minimum, minimum );
	}
}

float FloatRange::GetRandomInRange( RandomNumberGenerator& rng ) const
{
	return rng.RollRandomFloatInRange( minimum, maximum );
}

void FloatRange::Set( float newMinimum, float newMaximum )
{
	minimum = newMinimum;
	maximum = newMaximum;
}

bool FloatRange::SetFromText( const char* asText )
{
	Strings stringList = SplitStringOnDelimiter( asText, '~' );
	if( (int) stringList.size() == 1 )
	{
		float num = (float) atof( stringList[0].c_str() );
		minimum = num;
		maximum = num;
		return true;
	}
	if( (int) stringList.size() == 2 )
	{
		float firstNum = (float) atof( stringList[0].c_str() );
		float secondNum = (float) atof( stringList[1].c_str() );
		minimum = firstNum;
		maximum = secondNum;
		return true;
	}
	return false;
}
