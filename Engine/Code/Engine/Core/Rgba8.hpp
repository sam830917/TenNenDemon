#pragma once

struct Rgba8
{
public:
	unsigned char r = 255;
	unsigned char g = 255;
	unsigned char b = 255;
	unsigned char a = 255;

	static const Rgba8 WHITE;
	static const Rgba8 BLACK;
	static const Rgba8 RED;
	static const Rgba8 GREEN;
	static const Rgba8 BLUE;
	static const Rgba8 YELLOW;
	static const Rgba8 MAGENTA;

public:
	Rgba8();
	~Rgba8();
	explicit Rgba8( unsigned char initialR, unsigned char initialG, unsigned char initialB, unsigned char initialA );
	explicit Rgba8( unsigned char initialR, unsigned char initialG, unsigned char initialB );

	void ScaleAlpha( float alphaMultiplier );
	void SetFromText( const char* text );

	bool		operator==( const Rgba8& compare ) const;
};