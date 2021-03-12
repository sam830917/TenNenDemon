#include "Actor.hpp"
#include "Game/Map.hpp"
#include "Game/MapDefinition.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"

Actor::Actor( Game* theGame, Map* theMap, const Vec2& pos, const std::string& name, eEntityType entityType )
	:Entity( theGame, theMap, pos )
{
	m_entityDef = ActorDefinition::GetDefinitions( name );
	m_entityType = entityType;
	m_currentPositionIndex = theMap->m_mapDef->m_petStartIndex;
}

Actor::~Actor()
{
}

void Actor::Update( float deltaSeconds )
{
	Entity::Update( deltaSeconds );
}

void Actor::Render() const
{
	std::vector<Vertex_PCU> verts;
	SpriteAnimDefinition* animDef = m_entityDef->m_animSetDef->m_spriteAnims.at( "Idle" );
	SpriteDefinition sd = animDef->GetSpriteDefAtTime( (float) m_map->m_mapClock->GetTotalTime() );
	Vec2 uvAtMins, uvAtMaxs;
	sd.GetUVs( uvAtMins, uvAtMaxs );
	AABB2 localBounds = m_entityDef->m_localDrawBounds;
	AddVertsForAABB2( verts, localBounds, Rgba8::WHITE, uvAtMins, uvAtMaxs );
	g_theRenderer->BindTexture( &sd.GetTexture() );

	TransformVertexArray( (int)verts.size(), &verts[0], 1.f, 0.f, m_position );
	g_theRenderer->DrawVertexArray( (int)verts.size(), &verts[0] );

	DebugModeRender();
}

void Actor::DebugModeRender() const
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

void Actor::ChangePlayerSprite()
{
	switch( m_direction )
	{
	case DIRECTION_UP:
		m_currentSpriteName = m_isMoving ? "MoveNorth" : "IdleNorth";
		break;
	case DIRECTION_LEFT:
		m_currentSpriteName = m_isMoving ? "MoveWest" : "IdleWest";
		break;
	case DIRECTION_RIGHT:
		m_currentSpriteName = m_isMoving ? "MoveEast" : "IdleEast";
		break;
	case DIRECTION_DOWN:
		m_currentSpriteName = m_isMoving ? "MoveSouth" : "IdleSouth";
		break;
	default:
		break;
	}
	if ( m_isAttacking )
	{
		if ( m_timer->HasElapsed() )
		{
			m_isAttacking = false;
		}
	}
	//DebugAddMessage( 0.f, Rgba8::WHITE, "%d", m_isAttacking );
}

void Actor::UpdateFromKeyBoard( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	Vec2 translation = Vec2::ZERO;
	m_isMoving = false;
	m_position += translation;
}
