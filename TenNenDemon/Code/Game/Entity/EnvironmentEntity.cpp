#include "Game/Entity/EnvironmentEntity.hpp"
#include "Game/EnvironmentEntityDefinition.hpp"
#include "Game/Map.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"

EnvironmentEntity::EnvironmentEntity( Game* theGame, Map* theMap, const Vec2& pos, EnvironmentEntityDefinition* def )
	:Entity( theGame, theMap, pos )
{
	m_environmentEntityDef = def;
	m_entityDef = def->m_actorDef;
	m_entityType = def->m_entityType;
	m_currentPositionIndex = def->m_startIndex;
}

EnvironmentEntity::~EnvironmentEntity()
{
}

void EnvironmentEntity::Update( float deltaSeconds )
{
	Entity::Update( deltaSeconds );
}

void EnvironmentEntity::Render() const
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

void EnvironmentEntity::DebugModeRender() const
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
