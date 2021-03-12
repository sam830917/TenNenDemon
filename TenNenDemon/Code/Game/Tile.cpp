#include "Tile.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "TileDefinition.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

Tile::Tile( int tileX, int tileY, Vec2 startPoint )
	:m_tileCoords( tileX, tileY ),
	m_startPoint( startPoint )
{
}

Tile::~Tile()
{
}

void Tile::Update( float deltaSecons )
{
	UNUSED( deltaSecons );
}

void Tile::Render() const
{
	TileDefinition* tileDefinition = m_tileDef;

	if( tileDefinition )
	{
		Vec2 uvAtMins, uvAtMaxs;
		tileDefinition->GetUVs( uvAtMins, uvAtMaxs );
		std::vector<Vertex_PCU> verts;
		AddVertsForAABB2( verts, GetBounds(), tileDefinition->m_tint, uvAtMins, uvAtMaxs );
		TransformVertexArray( (int)verts.size(), &verts[0], 1.f, 0.f, Vec2::ZERO );
		g_theRenderer->BindTexture( g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Terrain_32x32.png" ) );
		g_theRenderer->DrawVertexArray( verts );
	}
}

void Tile::DebugModeRender() const
{
	std::vector<Vertex_PCU> verts;
	AABB2 tileRange = GetBounds();

	Vec2 mousePos = g_theInput->GetMouseNormalizedClientPos();
	mousePos = g_theGame->m_worldCamera->ClientToWorldPosition( mousePos, 0.f );
	Rgba8 color = Rgba8::YELLOW;
	if( IsPointInsideAABB2D( mousePos, tileRange ) )
	{
		color = Rgba8::RED;
		DebugAddMessage( 0.f, Rgba8::WHITE, "Tile Index ( %i, %i )", m_tileCoords.x, m_tileCoords.y );
	}
	AddVertsForAABB2( verts, tileRange.CarveBoxOffTop( 0.f, 0.02f ), color, Vec2::ZERO, Vec2::ONE );
	AddVertsForAABB2( verts, tileRange.CarveBoxOffBottom( 0.f, 0.02f ), color, Vec2::ZERO, Vec2::ONE );
	AddVertsForAABB2( verts, tileRange.CarveBoxOffLeft( 0.f, 0.02f ), color, Vec2::ZERO, Vec2::ONE );
	AddVertsForAABB2( verts, tileRange.CarveBoxOffRight( 0.f, 0.02f ), color, Vec2::ZERO, Vec2::ONE );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawVertexArray( verts );
}

void Tile::SetType( TileDefinition* tileDef )
{
	m_tileDef = tileDef;
}

AABB2 Tile::GetBounds() const
{
	Vec2 mins( (float)m_tileCoords.x, (float)m_tileCoords.y );
	Vec2 maxs( mins.x + 1.f, mins.y + 1.f );
	AABB2 bound = AABB2( mins, maxs );
	bound.Translate( m_startPoint );
	return bound;
}