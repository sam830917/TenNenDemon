#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "SpriteDefinition.hpp"
#include "Texture.hpp"
#include <vector>

class SpriteSheet
{
public:
	explicit SpriteSheet( const Texture& texture, const IntVec2& simpleGridLayout );

	const Texture&			GetTexture() const { return m_texture; }
	int						GetNumSprites() const { return (int) m_spriteDefs.size(); }
	const SpriteDefinition& GetSpriteDefinition( int spriteIndex ) const;
	void					GetspriteUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex ) const;
	const AABB2				GetTextureCoordsForSpriteCoords( const IntVec2& spriteIndex ) const;
	IntVec2					GetDimensions() const { return m_dimensions; }

protected:
	const Texture&					m_texture;
	IntVec2							m_dimensions;
	std::vector<SpriteDefinition>	m_spriteDefs;
};