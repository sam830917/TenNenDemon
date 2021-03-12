#include "Engine/Math/RandomNumberGenerator.hpp"
#include <stdlib.h>
#include "Vec2.hpp"

int RandomNumberGenerator::RollRandomIntLessThan( int maxNotInclusive )
{
	unsigned int randomBits = Get1dNoiseUint( m_position++, m_seed );
	return randomBits % maxNotInclusive;
}

int RandomNumberGenerator::RollRandomIntInRange( int minInclusive, int maxInclusive )
{
	int range = maxInclusive - minInclusive + 1;
	return minInclusive + RollRandomIntLessThan( range );
}

float RandomNumberGenerator::RollRandomFloatLessThan( float maxNotInclusive )
{
	double scale = (double)maxNotInclusive / (1.0 + (double)0xFFFFFFFF);
	unsigned int randomBits = Get1dNoiseUint( m_position++, m_seed );
	return (float)(scale * (double)randomBits);
}

float RandomNumberGenerator::RollRandomFloatZeroToAlmostOne()
{
	constexpr double scale = 1.0 / ( 1.0 + (double)0xFFFFFFFF );
	unsigned int randomBits = Get1dNoiseUint( m_position++, m_seed );
	return (float)(scale * (double)randomBits);
}

float RandomNumberGenerator::RollRandomFloatZeroToOneInclusive()
{
	constexpr double scale = 1.0 / (double) 0xFFFFFFFF;
	unsigned int randomBits = Get1dNoiseUint( m_position++, m_seed );
	return (float)( scale * (double) randomBits );
}

bool RandomNumberGenerator::RollPercentChance( float probabilityOfReturningTrue )
{
	float randomNumber = RollRandomFloatZeroToOneInclusive();
	if ( probabilityOfReturningTrue >= randomNumber )
	{
		return true;
	}
	else
	{
		return false;
	}
}

Vec2 RandomNumberGenerator::RollRandomDirection2D()
{
	Vec2 direction = Vec2( 1.f, 0.f );
	direction.SetAngleDegrees( RollRandomFloatInRange( 0.f, 360.f ) );
	return direction;
}

void RandomNumberGenerator::Reset( unsigned int seed )
{
	m_seed = seed;
}

float RandomNumberGenerator::RollRandomFloatInRange( float minInclusive, float maxInclusive )
{
	float range = maxInclusive - minInclusive;
	return minInclusive + RollRandomFloatLessThan( range );
}
