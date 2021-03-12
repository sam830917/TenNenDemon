#include "Image.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include "ThirdParty/stb/stb_image.h"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"

Image::Image( const char* imageFilePath, bool isFlipVertically )
	:m_imageFilePath( imageFilePath )
{
	int imageTexelSizeX = 0; // This will be filled in for us to indicate image width
	int imageTexelSizeY = 0; // This will be filled in for us to indicate image height
	int numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 0; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)

	// Load (and decompress) the image RGB(A) bytes from a file on disk into a memory buffer (array of bytes)
	if ( isFlipVertically )
	{
		stbi_set_flip_vertically_on_load( 1 );
	}
	else
	{
		stbi_set_flip_vertically_on_load( 0 );
	}
	unsigned char* imageData = stbi_load( imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested );

	// Check if the load was successful
	GUARANTEE_OR_DIE( imageData, Stringf( "Failed to load image \"%s\"", imageFilePath ) );
	GUARANTEE_OR_DIE( numComponents == 4 || numComponents == 3 && imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, numComponents, imageTexelSizeX, imageTexelSizeY ) );

	unsigned int bytePerPixel = numComponents;
	int size = imageTexelSizeY * imageTexelSizeX;
	std::vector<unsigned char> c( imageData, imageData+size );
	for ( int index = 0; index < size; index++ )
	{
		Rgba8 color = Rgba8();
		unsigned char* pixelOffset = imageData + (index * bytePerPixel);
		color.r = pixelOffset[0];
		color.g = pixelOffset[1];
		color.b = pixelOffset[2];

		if( numComponents == 4 )
		{
			color.a = pixelOffset[3];
		}

		m_rgbaTexels.push_back( color );
	}
	m_dimensions = IntVec2( imageTexelSizeX, imageTexelSizeY );

	stbi_image_free( imageData );
}

Image::Image( const Image& copyFrom, int numRotations )
{
	m_imageFilePath = copyFrom.m_imageFilePath;

	// 90 degrees
	if ( numRotations == 1 )
	{
		for ( int xIndex = 0; xIndex < copyFrom.m_dimensions.x; xIndex++ )
		{
			for( int yIndex = copyFrom.m_dimensions.y - 1; yIndex >= 0; yIndex-- )
			{
				m_rgbaTexels.push_back( copyFrom.GetTexelColor( xIndex, yIndex ) );
			}
		}
		m_dimensions = IntVec2( copyFrom.m_dimensions.y, copyFrom.m_dimensions.x );
	}
	// 180 degrees
	else if ( numRotations == 2 )
	{
		m_dimensions = copyFrom.m_dimensions;
		for( int yIndex = m_dimensions.y - 1; yIndex >= 0; yIndex-- )
		{
			for( int xIndex = m_dimensions.x - 1; xIndex >= 0; xIndex-- )
			{
				m_rgbaTexels.push_back( copyFrom.GetTexelColor( xIndex, yIndex ) );
			}
		}
	}
	// 270 degrees
	else if( numRotations == 3 )
	{
		for( int xIndex = copyFrom.m_dimensions.x - 1; xIndex >= 0; xIndex-- )
		{
			for( int yIndex = 0; yIndex < copyFrom.m_dimensions.y; yIndex++ )
			{
				m_rgbaTexels.push_back( copyFrom.GetTexelColor( xIndex, yIndex ) );
			}
		}
		m_dimensions = IntVec2( copyFrom.m_dimensions.y, copyFrom.m_dimensions.x );
	}
	// 0 degree
	else
	{
		m_dimensions = copyFrom.m_dimensions;
		m_rgbaTexels = copyFrom.m_rgbaTexels;
	}
}

const std::string& Image::GetImageFilePath() const
{
	return m_imageFilePath;
}

IntVec2 Image::GetDimensions() const
{
	return m_dimensions;
}

Rgba8 Image::GetTexelColor( int texelIndex ) const
{
	return m_rgbaTexels[texelIndex];
}

Rgba8 Image::GetTexelColor( int texelX, int texelY ) const
{
	int index = texelX + ( m_dimensions.x * texelY );
	return m_rgbaTexels[index];
}

Rgba8 Image::GetTexelColor( const IntVec2& texelCoords ) const
{
	int index = texelCoords.x + (m_dimensions.x * texelCoords.y);
	return m_rgbaTexels[index];
}

unsigned char* Image::GetTexelsBuffer()
{
	const int size = (int) m_rgbaTexels.size();
	unsigned int bytePerPixel = 4;
	unsigned char* texels = new unsigned char[size * bytePerPixel];
	for ( int i = 0; i < size; ++i )
	{
		Rgba8 color = m_rgbaTexels[i];
		unsigned char* pixelOffset = texels + (i * bytePerPixel);
		pixelOffset[0] = color.r;
		pixelOffset[1] = color.g;
		pixelOffset[2] = color.b;
		pixelOffset[3] = color.a;
	}

	return texels;
}

void Image::SetTexelColor( int texelX, int texelY, const Rgba8& newColor )
{
	int index = texelX + ( m_dimensions.x * texelY );
	m_rgbaTexels[index] = newColor;
}

void Image::SetTexelColor( const IntVec2& texelCoords, const Rgba8& newColor )
{
	int index = texelCoords.x + (m_dimensions.x * texelCoords.y);
	m_rgbaTexels[index] = newColor;
}
