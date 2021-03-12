#include "Player.hpp"
#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/World.hpp"
#include "Game/MapDefinition.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/ParticleSystem.hpp"
#include "Engine/Renderer/DebugRender.hpp"

Player::Player( Game* theGame, Map* theMap, const Vec2& pos, const std::string& name )
	:Entity( theGame, theMap, pos )
{
	m_entityDef = ActorDefinition::GetDefinitions( name );
	m_currentPositionIndex = theMap->m_mapDef->m_playerStartIndex;
	m_entityType = ENTITY_TYPE_PLAYER;
}

Player::~Player()
{
}

void Player::Update( float deltaSeconds )
{
	HandlePlayerInput( deltaSeconds );
	Entity::Update( deltaSeconds );
	if ( m_isMoving )
	{
		m_map->m_playerDust->m_stopCreatingParticle = false;
	}
	else
	{
		m_map->m_playerDust->m_stopCreatingParticle = true;
	}
}

void Player::Render() const
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

void Player::DebugModeRender() const
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

void Player::HandlePlayerInput( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	if ( m_isMoving || !m_game->m_world->IsPlayerControllable() )
	{
		return;
	}
	// move input
	if( g_theInput->WasKeyJustPressed( 'W' ) )
	{
		IntVec2 targetIndex = m_currentPositionIndex + IntVec2( 0, 1 );
		m_map->MovePlayerIfPossible( IntVec2( 0, 1 ) );

		//DebugAddMessage( 1.f, Rgba8::YELLOW, Rgba8::BLACK, "Player Input : W" );
	}
	else if( g_theInput->WasKeyJustPressed( 'S' ) )
	{
		IntVec2 targetIndex = m_currentPositionIndex + IntVec2( 0, -1 );
		m_map->MovePlayerIfPossible( IntVec2( 0, -1 ) );

		//DebugAddMessage( 1.f, Rgba8::YELLOW, Rgba8::BLACK, "Player Input : S" );
	}
	else if( g_theInput->WasKeyJustPressed( 'A' ) )
	{
		IntVec2 targetIndex = m_currentPositionIndex + IntVec2( -1, 0 );
		m_map->MovePlayerIfPossible( IntVec2( -1, 0 ) );

		//DebugAddMessage( 1.f, Rgba8::YELLOW, Rgba8::BLACK, "Player Input : A" );
	}
	else if( g_theInput->WasKeyJustPressed( 'D' ) )
	{
		IntVec2 targetIndex = m_currentPositionIndex + IntVec2( 1, 0 );
		m_map->MovePlayerIfPossible( IntVec2( 1, 0 ) );
	}
	// fireball input
	else if( g_theInput->WasKeyJustPressed( VK_UP ) )
	{
		m_map->UseFireballAbility( IntVec2( 0, 1 ) );
	}
	else if( g_theInput->WasKeyJustPressed( VK_RIGHT ) )
	{
		m_map->UseFireballAbility( IntVec2( 1, 0 ) );
	}
	else if( g_theInput->WasKeyJustPressed( VK_LEFT ) )
	{
		m_map->UseFireballAbility( IntVec2( -1, 0 ) );
	}
	else if( g_theInput->WasKeyJustPressed( VK_DOWN ) )
	{
		m_map->UseFireballAbility( IntVec2( 0, -1 ) );
	}
}
