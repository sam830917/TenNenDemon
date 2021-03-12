#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <string>
#include <vector>

class Image
{
public:
	Image( const char* imageFilePath, bool isFlipVertically = true );
	// Rotation : 0 = 0 degree, 1 = 90 degrees, 2 = 180 degrees, 3 = 270 degrees
	Image( const Image& copyFrom, int numRotations = 0 );

	const std::string&	GetImageFilePath() const;
	IntVec2				GetDimensions() const;
	Rgba8				GetTexelColor( int texelIndex ) const;
	Rgba8				GetTexelColor( int texelX, int texelY ) const;
	Rgba8				GetTexelColor( const IntVec2& texelCoords ) const;
	unsigned char*		GetTexelsBuffer();

	void				SetTexelColor( int texelX, int texelY, const Rgba8& newColor );
	void				SetTexelColor( const IntVec2& texelCoords, const Rgba8& newColor );

private:
	std::string					m_imageFilePath;
	IntVec2						m_dimensions = IntVec2( 0, 0 );
	std::vector< Rgba8 >		m_rgbaTexels;
};
