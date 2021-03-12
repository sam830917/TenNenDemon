#pragma once
#include "SpriteSheet.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <vector>
#include <string>

class RenderContext;

class BitmapFont
{
	friend class RenderContext; // Only the RenderContext can create new BitmapFont objects!

private:
	BitmapFont( const char* fontName, const Texture* fontTexture );
	BitmapFont( const char* fontName, const Texture* fontTexture, const IntVec2& simpleGridLayout );

public:
	const Texture* GetTexture() const;

	void AddVertsForText2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& textMins,
		float cellHeight, const std::string& text, const Rgba8& tint=Rgba8::WHITE, float cellAspect=1.f );

	void AddVertsForText3D( std::vector<Vertex_PCU>& vertexArray, const Vec3& textMins,
		float cellHeight, const std::string& text, const Rgba8& tint=Rgba8::WHITE, float cellAspect=1.f );

	void AddVertsForTextInBox2D( std::vector<Vertex_PCU>& vertexArray, const AABB2& box, float cellHeight,
		const std::string& text, const Rgba8& tint=Rgba8::WHITE, float cellAspect=1.f,
		const Vec2& alignment=ALIGN_CENTERED );
	Vec2 GetDimensionsForText2D( float cellHeight, const std::string& text, float cellAspect=1.f );


protected:
	float GetGlyphAspect( int glyphUnicode ) const; // For now this will always return 1.0f!!!

protected:
	std::string	m_fontName;
	SpriteSheet	m_glyphSpriteSheet;

};