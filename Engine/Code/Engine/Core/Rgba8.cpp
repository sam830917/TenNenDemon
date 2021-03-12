#include "Rgba8.hpp"
#include <math.h>
#include <string>
#include "Engine/Core/StringUtils.hpp"

const Rgba8 Rgba8::WHITE	= Rgba8( 255, 255, 255 );
const Rgba8 Rgba8::BLACK	= Rgba8( 0, 0, 0 );
const Rgba8 Rgba8::RED		= Rgba8( 255, 0, 0 );
const Rgba8 Rgba8::GREEN	= Rgba8( 0, 255, 0 );
const Rgba8 Rgba8::BLUE		= Rgba8( 0, 0, 255 );
const Rgba8 Rgba8::YELLOW	= Rgba8( 255, 255, 0 );
const Rgba8 Rgba8::MAGENTA	= Rgba8( 255, 0, 255 );

Rgba8::Rgba8()
{
}

Rgba8::~Rgba8()
{
}

Rgba8::Rgba8( unsigned char initialR, unsigned char initialG, unsigned char initialB, unsigned char initialA )
	: r( initialR )
	, g( initialG )
	, b( initialB )
	, a( initialA )
{
}

Rgba8::Rgba8( unsigned char initialR, unsigned char initialG, unsigned char initialB )
	: r( initialR )
	, g( initialG )
	, b( initialB ) 
{
}

void Rgba8::ScaleAlpha( float alphaMultiplier )
{
	a = static_cast<unsigned char>( alphaMultiplier * static_cast<float>( a ) );
}

void Rgba8::SetFromText( const char* text )
{
	Strings stringList = SplitStringOnDelimiter( text, ',' );
	if( stringList.size() >= 3 )
	{
		float rNum = (float) atof( stringList[0].c_str() );
		float gNum = (float) atof( stringList[1].c_str() );
		float bNum = (float) atof( stringList[2].c_str() );
		r = static_cast<unsigned char>(rNum);
		g = static_cast<unsigned char>(gNum);
		b = static_cast<unsigned char>(bNum);
		if( stringList.size() == 4 )
		{
			float aNum = (float)atof( stringList[3].c_str() );
			a = static_cast<unsigned char>(aNum);
		}
		else
		{
			a = 255;
		}
	}
}

bool Rgba8::operator==( const Rgba8& compare ) const
{
	if ( r == compare.r && g == compare.g && b == compare.b && a == compare.a )
	{
		return true;
	}
	return false;
}
