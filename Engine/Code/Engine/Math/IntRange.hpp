#pragma once
#include <string>

class RandomNumberGenerator;

struct IntRange
{
public:
	int minimum = 0;
	int maximum = 0;

public:
	IntRange() = default;
	explicit IntRange( int minAndMax );			// construct from single int [N,N];
	explicit IntRange( int min, int max );		// construct from [min,max]
	explicit IntRange( const char* asText );	// construct from "5" or "12~19"

	// Accessors
	bool			IsInRange( int value ) const;
	bool			DoesOverlap( const IntRange& otherRange ) const;
	std::string		GetAsString() const; // returns "6" or "21~35", etc.
	int				GetRandomInRange( RandomNumberGenerator& rng ) const;

	// Mutators
	void			Set( int newMinimum, int newMaximum );
	bool			SetFromText( const char* asText ); // return false if malformatted
	
};