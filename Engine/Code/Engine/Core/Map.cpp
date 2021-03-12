#include "Map.hpp"

Map::Map()
{
}

Map::~Map()
{
}

int Map::GetTileIndexForTileCoords( const IntVec2& tileCoords ) const
{
	return tileCoords.x + ( m_size.x * tileCoords.y );
}

IntVec2 Map::GetTileCoordsForTileIndex( int tileIndex ) const
{
	return m_tiles[ tileIndex ].GetTileCoords();
}
