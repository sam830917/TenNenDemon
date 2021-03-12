#include "Game/Entity/Fireball.hpp"
#include "Game/Map.hpp"
#include "Game/MapDefinition.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"

Fireball::Fireball( Game* theGame, Map* theMap, const Vec2& pos, const std::string& name )
	:Entity( theGame, theMap, pos )
{
	m_entityDef = ActorDefinition::GetDefinitions( name );
	m_currentPositionIndex = theMap->m_mapDef->m_playerStartIndex;
	m_entityType = ENTITY_TYPE_FIREBALL;
	m_speed = 10.f;
}

Fireball::~Fireball()
{
}

void Fireball::Update( float deltaSeconds )
{
	if ( !m_isMoving )
	{
		m_isMoving = true;
		m_map->MoveFireballIfPossible( this, m_movingDirection );
	}
	Entity::Update( deltaSeconds );
}

void Fireball::Render() const
{
	std::vector<Vertex_PCU> verts;
	SpriteAnimDefinition* animDef = m_entityDef->m_animSetDef->m_spriteAnims.at( m_spriteName );
	SpriteDefinition sd = animDef->GetSpriteDefAtTime( (float)m_map->m_mapClock->GetTotalTime() );
	Vec2 uvAtMins, uvAtMaxs;
	sd.GetUVs( uvAtMins, uvAtMaxs );
	AABB2 localBounds = m_entityDef->m_localDrawBounds;
	AddVertsForAABB2( verts, localBounds, Rgba8::WHITE, uvAtMins, uvAtMaxs );
	g_theRenderer->BindTexture( &sd.GetTexture() );

	TransformVertexArray( (int)verts.size(), &verts[0], 1.f, 0.f, m_position );
	g_theRenderer->DrawVertexArray( (int)verts.size(), &verts[0] );

	DebugModeRender();
}

void Fireball::DebugModeRender() const
{
	if( !g_isDebugMode )
		return;

	g_theRenderer->BindTexture( nullptr );
	AABB2 localBounds = AABB2( m_entityDef->m_localDrawBounds.mins + m_position, m_entityDef->m_localDrawBounds.maxs + m_position );
	g_theRenderer->DrawLine( localBounds.mins, Vec2( localBounds.maxs.x, localBounds.mins.y ), Rgba8::RED, 0.02f );
	g_theRenderer->DrawLine( localBounds.mins, Vec2( localBounds.mins.x, localBounds.maxs.y ), Rgba8::RED, 0.02f );
	g_theRenderer->DrawLine( Vec2( localBounds.maxs.x, localBounds.mins.y ), localBounds.maxs, Rgba8::RED, 0.02f );
	g_theRenderer->DrawLine( Vec2( localBounds.mins.x, localBounds.maxs.y ), localBounds.maxs, Rgba8::RED, 0.02f );
}

void Fireball::SetMovingDirection( const IntVec2& movingDirection )
{
	m_movingDirection = movingDirection;
	if ( movingDirection == IntVec2( 1, 0 ) )
	{
		m_spriteName = "East";
	}
	else if ( movingDirection == IntVec2( -1, 0 ) )
	{
		m_spriteName = "West";
	}
	else if ( movingDirection == IntVec2( 0, 1 ) )
	{
		m_spriteName = "North";
	}
	else if ( movingDirection == IntVec2( 0, -1 ) )
	{
		m_spriteName = "South";
	}
}
