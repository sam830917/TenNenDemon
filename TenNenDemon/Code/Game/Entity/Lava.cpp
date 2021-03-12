#include "Game/Entity/Lava.hpp"
#include "Game/EnvironmentEntityDefinition.hpp"
#include "Game/Map.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/ParticleSystem.hpp"

Lava::Lava( Game* theGame, Map* theMap, const Vec2& pos, EnvironmentEntityDefinition* def )
	:Entity( theGame, theMap, pos )
{
	m_environmentEntityDef = def;
	m_entityDef = def->m_actorDef;
	m_entityType = ENTITY_TYPE_LAVA;
	m_currentPositionIndex = def->m_startIndex;
	m_emitter = m_map->m_particleSystem->CreateEmitter();
	m_emitter->m_position = Vec3( m_position, 0.f );
	m_emitter->m_color = Rgba8( 100, 100, 100, 200 );
	m_emitter->m_hasFadeOut = true;
	m_emitter->m_minOrientation = 90.f;
	m_emitter->m_maxOrientation = 90.f;
	m_emitter->m_scale = Vec3( 0.5f, 0.5f, 1.f );
	m_emitter->m_velocity = 0.5f;
	m_emitter->m_interval = 0.1f;
	m_emitter->m_spawnOffsetMin = Vec3( -0.6f, -0.5f, 0.f );
	m_emitter->m_spawnOffsetMax = Vec3( 0.3f, 0.5f, 0.f );
	m_emitter->m_texture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/steam-texture.png" );
}

Lava::~Lava()
{
}

void Lava::Update( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}

void Lava::Render() const
{
	std::vector<Vertex_PCU> verts;
	std::string spriteName = "Normal";
	if ( m_isFilled )
	{
		spriteName = "Filled";
		m_emitter->m_stopCreatingParticle = true;
	}
	SpriteAnimDefinition* animDef = m_entityDef->m_animSetDef->m_spriteAnims.at( spriteName );
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

void Lava::DebugModeRender() const
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
