#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "TileDefinition.hpp"

class Tile
{
public:
	Tile( int tileX, int tileY, Vec2 startPoint );
	~Tile();

	void Update( float deltaSecons );
	void Render() const;
	void DebugModeRender() const;

	void SetType( TileDefinition* tileDef );
	AABB2 GetBounds() const;

public:
	IntVec2 m_tileCoords;
	Vec2 m_startPoint = Vec2::ZERO;
	TileDefinition*	m_tileDef = nullptr;
};