#include "SpriteSheet.hpp"
#include "SpriteDefinition.hpp"

SpriteSheet::SpriteSheet( const Texture& texture, const IntVec2& simpleGridLayout )
	:m_texture( texture ),
	m_dimensions( simpleGridLayout )
{
	float uPerSpriteGridX = 1.0f / static_cast<float>( simpleGridLayout.x );
	float vPerSpriteGridY = 1.0f / static_cast<float>( simpleGridLayout.y );

	int numSprites = simpleGridLayout.x * simpleGridLayout.y;
	const int& spritePerRow = simpleGridLayout.x;
	m_dimensions = simpleGridLayout;

	for ( int spriteIndex = 0;spriteIndex < numSprites;spriteIndex++ )
	{
		int spriteGridX = spriteIndex % spritePerRow;
		int spriteGridY = spriteIndex / spritePerRow;
		float uAtMinX = uPerSpriteGridX * static_cast<float>( spriteGridX );
		float vAtMaxY = 1.0f - vPerSpriteGridY * static_cast<float>( spriteGridY );
		float uAtMaxX = uAtMinX + uPerSpriteGridX;
		float vAtMinY = vAtMaxY - vPerSpriteGridY;
		Vec2 uvAtMins( uAtMinX, vAtMinY );
		Vec2 uvAtMaxs( uAtMaxX, vAtMaxY );
		m_spriteDefs.push_back( SpriteDefinition( *this, spriteIndex, uvAtMins, uvAtMaxs ) );
	}
}

const SpriteDefinition& SpriteSheet::GetSpriteDefinition( int spriteIndex ) const
{
	return m_spriteDefs[ spriteIndex ];
}

void SpriteSheet::GetspriteUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex ) const
{
	const SpriteDefinition& spriteDefinition = m_spriteDefs[ spriteIndex ];
	spriteDefinition.GetUVs( out_uvAtMins, out_uvAtMaxs );
}

const AABB2 SpriteSheet::GetTextureCoordsForSpriteCoords( const IntVec2& spriteIndex ) const
{
	//const int& index = ( ( m_dimensions.y - 1 ) - spriteIndex.x ) * m_dimensions.x + spriteIndex.y;
	const int& index = m_dimensions.y * spriteIndex.x + spriteIndex.y;
	SpriteDefinition sd = GetSpriteDefinition( index );
	Vec2 uvAtMins;
	Vec2 uvAtMaxs;
	sd.GetUVs( uvAtMins, uvAtMaxs );

	return AABB2( uvAtMins, uvAtMaxs );
}