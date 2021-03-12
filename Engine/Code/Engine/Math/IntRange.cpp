#include "IntRange.hpp"
#include "RandomNumberGenerator.hpp"
#include <math.h>
#include "Engine/Core/StringUtils.hpp"

IntRange::IntRange( int minAndMax )
{
	minimum = minAndMax;
	maximum = minAndMax;
}

IntRange::IntRange( int min, int max )
{
	minimum = min;
	maximum = max;
}

IntRange::IntRange( const char* asText )
{
	Strings stringList = SplitStringOnDelimiter( asText, '~' );
	if ( stringList.size() == 1 )
	{
		int num = atoi( stringList[0].c_str() );
		minimum = num;
		maximum = num;
	}
	if( stringList.size() == 2 )
	{
		int firstNum = atoi( stringList[0].c_str() );
		int secondNum = atoi( stringList[1].c_str() );
		minimum = firstNum;
		maximum = secondNum;
	}
}

bool IntRange::IsInRange( int value ) const
{
	if ( minimum <= value && maximum >= value )
	{
		return true;
	}
	return false;
}

bool IntRange::DoesOverlap( const IntRange& otherRange ) const
{
	if ( IsInRange( otherRange.minimum ) || IsInRange( maximum ) )
	{
		return true;
	}
	return false;
}

std::string IntRange::GetAsString() const
{
	if ( maximum == minimum )
	{
		return std::to_string(minimum);
	}
	else
	{
		return Stringf( "%i~%i", minimum, maximum );
	}
}

int IntRange::GetRandomInRange( RandomNumberGenerator& rng ) const
{
	return rng.RollRandomIntInRange( minimum, maximum );
}

void IntRange::Set( int newMinimum, int newMaximum )
{
	minimum = newMinimum;
	maximum = newMaximum;
}

bool IntRange::SetFromText( const char* asText )
{
	Strings stringList = SplitStringOnDelimiter( asText, '~' );
	if( stringList.size() == 1 )
	{
		int num = atoi( stringList[0].c_str() );
		minimum = num;
		maximum = num;
		return true;
	}
	if( stringList.size() == 2 )
	{
		int firstNum = atoi( stringList[0].c_str() );
		int secondNum = atoi( stringList[1].c_str() );
		minimum = firstNum;
		maximum = secondNum;
		return true;
	}
	return false;
}
