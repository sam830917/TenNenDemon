#pragma once
#include <vector>
#include "Tile.hpp"
#include "Engine/Math/IntVec2.hpp"

class Map
{
public:
	Map();
	~Map();

	int GetTileIndexForTileCoords( const IntVec2& tileCoords ) const;
	IntVec2 GetTileCoordsForTileIndex( int tileIndex ) const;

private:
	std::vector< Tile >       m_tiles; // Note: this is NOT a 2D array!
	IntVec2 m_size;
};