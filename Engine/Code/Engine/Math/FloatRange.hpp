#pragma once
#include <string>

class RandomNumberGenerator;

struct FloatRange
{
public:
	float minimum = 0.f;
	float maximum = 1.f;

public:
	FloatRange() = default;
	explicit FloatRange( float minAndMax );			// construct from single int [N,N];
	explicit FloatRange( float min, float max );		// construct from [min,max]
	explicit FloatRange( const char* asText );		// construct from "5" or "12.1~19.5"

	// Accessors
	bool			IsInRange( float value ) const;
	bool			DoesOverlap( const FloatRange& otherRange ) const;
	std::string		GetAsString() const; // returns "6" or "21.123~35.567", etc.
	float			GetRandomInRange( RandomNumberGenerator& rng ) const;

	// Mutators
	void			Set( float newMinimum, float newMaximum );
	bool			SetFromText( const char* asText ); // return false if malformatted

};