#pragma once
#include "Engine/Math/RawNoise.hpp"

struct Vec2;

class RandomNumberGenerator
{
public:
	int		RollRandomIntLessThan( int maxNotInclusive );
	int		RollRandomIntInRange( int minInclusive, int maxInclusive );
	float	RollRandomFloatLessThan( float maxNotInclusive );
	float	RollRandomFloatInRange( float minInclusive, float maxInclusive );
	float	RollRandomFloatZeroToOneInclusive();
	float	RollRandomFloatZeroToAlmostOne();
	bool	RollPercentChance( float probabilityOfReturningTrue );
	Vec2	RollRandomDirection2D();

	void	Reset( unsigned int seed = 0 );

private:
	unsigned int	m_seed = 0;			//we'll use this later when we replace rand() using noise
	int				m_position = 0;
};
