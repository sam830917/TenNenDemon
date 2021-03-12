#include "BitmapFont.hpp"
#include "RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"

BitmapFont::BitmapFont( const char* fontName, const Texture* fontTexture )
	:m_fontName(fontName),
	m_glyphSpriteSheet( SpriteSheet( *fontTexture, IntVec2( 16, 16 ) ) )
{
}

BitmapFont::BitmapFont( const char* fontName, const Texture* fontTexture, const IntVec2& simpleGridLayout )
	: m_fontName( fontName ),
	m_glyphSpriteSheet( SpriteSheet( *fontTexture, simpleGridLayout ) )
{
}

const Texture* BitmapFont::GetTexture() const
{
	return &m_glyphSpriteSheet.GetTexture();
}

void BitmapFont::AddVertsForText2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& textMins, 
	float cellHeight, const std::string& text, const Rgba8& tint, float cellAspect )
{
	for( int textIndex = 0;textIndex < text.length();textIndex++ ) 
	{
		char t = text[textIndex];

		float cellWidth = cellHeight * GetGlyphAspect( t ) * cellAspect;
		SpriteDefinition sd = m_glyphSpriteSheet.GetSpriteDefinition( t );
		Vec2 uvMins;
		Vec2 uvMaxs;
		sd.GetUVs( uvMins, uvMaxs );
		Vec2 minVec2 = textMins + Vec2( cellWidth, 0.f ) * (float) textIndex;
		Vec2 maxVec2 = minVec2 + Vec2( cellWidth, cellHeight );

		vertexArray.push_back( Vertex_PCU( Vec3( minVec2.x, minVec2.y, 0.f ), tint, Vec2( uvMins.x, uvMins.y ) ) );
		vertexArray.push_back( Vertex_PCU( Vec3( maxVec2.x, minVec2.y, 0.f ), tint, Vec2( uvMaxs.x, uvMins.y ) ) );
		vertexArray.push_back( Vertex_PCU( Vec3( minVec2.x, maxVec2.y, 0.f ), tint, Vec2( uvMins.x, uvMaxs.y ) ) );

		vertexArray.push_back( Vertex_PCU( Vec3( maxVec2.x, minVec2.y, 0.f ), tint, Vec2( uvMaxs.x, uvMins.y ) ) );
		vertexArray.push_back( Vertex_PCU( Vec3( maxVec2.x, maxVec2.y, 0.f ), tint, Vec2( uvMaxs.x, uvMaxs.y ) ) );
		vertexArray.push_back( Vertex_PCU( Vec3( minVec2.x, maxVec2.y, 0.f ), tint, Vec2( uvMins.x, uvMaxs.y ) ) );
	}
}

void BitmapFont::AddVertsForText3D( std::vector<Vertex_PCU>& vertexArray, const Vec3& textMins, float cellHeight, const std::string& text, const Rgba8& tint, float cellAspect )
{
	for( int textIndex = 0; textIndex < text.length(); textIndex++ )
	{
		char t = text[textIndex];

		float cellWidth = cellHeight * GetGlyphAspect( t ) * cellAspect;
		SpriteDefinition sd = m_glyphSpriteSheet.GetSpriteDefinition( t );
		Vec2 uvMins;
		Vec2 uvMaxs;
		sd.GetUVs( uvMins, uvMaxs );
		Vec3 minVec3 = textMins + Vec3( cellWidth, 0.f, 0.f ) * (float)textIndex;
		Vec3 maxVec3 = minVec3 + Vec3( cellWidth, cellHeight, minVec3.z );

		vertexArray.push_back( Vertex_PCU( Vec3( minVec3.x, minVec3.y, textMins.z ), tint, Vec2( uvMins.x, uvMins.y ) ) );
		vertexArray.push_back( Vertex_PCU( Vec3( maxVec3.x, minVec3.y, textMins.z ), tint, Vec2( uvMaxs.x, uvMins.y ) ) );
		vertexArray.push_back( Vertex_PCU( Vec3( minVec3.x, maxVec3.y, textMins.z ), tint, Vec2( uvMins.x, uvMaxs.y ) ) );

		vertexArray.push_back( Vertex_PCU( Vec3( maxVec3.x, minVec3.y, textMins.z ), tint, Vec2( uvMaxs.x, uvMins.y ) ) );
		vertexArray.push_back( Vertex_PCU( Vec3( maxVec3.x, maxVec3.y, textMins.z ), tint, Vec2( uvMaxs.x, uvMaxs.y ) ) );
		vertexArray.push_back( Vertex_PCU( Vec3( minVec3.x, maxVec3.y, textMins.z ), tint, Vec2( uvMins.x, uvMaxs.y ) ) );
	}
}

void BitmapFont::AddVertsForTextInBox2D( std::vector<Vertex_PCU>& vertexArray, const AABB2& box,
	float cellHeight, const std::string& text, const Rgba8& tint, float cellAspect, const Vec2& alignment )
{
	float totalWidth = 0.f;
	for( int textIndex = 0; textIndex < text.length(); textIndex++ )
	{
		char t = text[textIndex];

		float cellWidth = cellHeight * GetGlyphAspect( t ) * cellAspect;
		totalWidth += cellWidth;
	}
	AABB2 textAABB2 = AABB2( Vec2(), Vec2( totalWidth, cellHeight ) );
	Vec2 boxSize = Vec2( box.maxs - box.mins );
	textAABB2.Translate( Vec2( (boxSize.x - totalWidth) * alignment.x, (boxSize.y - cellHeight) * alignment.y ) );
	textAABB2.Translate( box.mins );

	AddVertsForText2D( vertexArray, textAABB2.mins, cellHeight, text, tint, cellAspect );
}

Vec2 BitmapFont::GetDimensionsForText2D( float cellHeight, const std::string& text, float cellAspect )
{
	float totalWidth = 0.f;
	for( int textIndex = 0; textIndex < text.length(); textIndex++ )
	{
		char t = text[textIndex];

		float cellWidth = cellHeight * GetGlyphAspect( t ) * cellAspect;
		totalWidth += cellWidth;
	}
	return Vec2( totalWidth, cellHeight);
}

float BitmapFont::GetGlyphAspect( int glyphUnicode ) const
{
	UNUSED(glyphUnicode);
	return 1.0f;
}
