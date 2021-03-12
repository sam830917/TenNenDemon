#include "Game/World.hpp"
#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/CutsceneDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/Timer.hpp"

World::World( Game* game )
	:m_game( game )
{
}

World::~World()
{
	delete m_currentMap;
	m_currentMap = nullptr;
}

void World::Update( float deltaSeconds )
{
	m_currentMap->Update( deltaSeconds );
	if ( m_isJustWinCurrentLevel )
	{
		if( m_currentMap->m_successTimer->HasElapsed() )
		{
			if ( !m_currentMap->m_cutscenePlayer.IsPlayingCutscene() )
			{
				if( m_currentMap->m_mapDef->m_afterLevelCutscene )
				{
					m_currentMap->PlayCutscene( m_currentMap->m_mapDef->m_afterLevelCutscene, "next_map" );
				}
				else
				{
					NextMap();
				}
			}
		}
	}

	if( m_isExitAlertShowing )
	{
		if ( g_theInput->WasKeyJustPressed( VK_RETURN ) )
		{
			m_isExitAlertShowing = false;
			g_theGame->ChangeGameState( MAIN_MENU );
		}
		if ( g_theInput->WasKeyJustPressed( VK_ESCAPE ) )
		{
			m_isExitAlertShowing = false;
		}
	}
	else
	{
		if( g_theInput->WasKeyJustPressed( VK_ESCAPE ) )
		{
			!m_isExitAlertShowing ? m_isExitAlertShowing = true : true;
		}
	}
}

void World::Render() const
{
	m_currentMap->Render();

	if ( m_isExitAlertShowing )
	{
		AABB2 screenBounds = DebugGetScreenBounds();
		AABB2 alertBounds = AABB2( Vec2::ZERO, Vec2( 600.f, 200.f ) );
		alertBounds.TranslateAtPivot( screenBounds, Vec4( 0.5f, 0.5f, 0.f, 0.f ), ALIGN_CENTERED );

		DebugAddScreenTexturedQuad( alertBounds, nullptr, AABB2::ZERO_TO_ONE, Rgba8(0, 0, 0, 230), 0.f );
		DebugAddScreenTextf( Vec4( 0.5f, 0.5f, 0.f, 0.f ), ALIGN_CENTERED, 20.f, Rgba8(255, 255, 255, 230), 0.f, "EXIT[Enter]  REMAIN[Esc]" );
	}
}

void World::DebugModeRender() const
{
	m_currentMap->DebugModeRender();
}

void World::EndFrame()
{
	m_currentMap->ClearDeadEntities();
}

void World::CreateNewMap( const std::string& mapName )
{
	Map* newMap = new Map( m_game, mapName );
	if ( m_currentMap == nullptr )
	{
		m_currentMap = newMap;
		m_currentMapName = mapName;
	}
}

void World::WinCurrentLevel()
{
	m_currentMap->m_successTimer->SetSeconds( m_currentMap->m_mapClock, 2.f );
	g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/dialogue_success_01.mp3" ), false, g_theGame->m_volume );
	m_isJustWinCurrentLevel = true;
}

void World::RestartCurrentMap( bool needPlayCutscene  )
{
	if ( m_currentMap )
	{
		delete m_currentMap;
		m_currentMap = nullptr;
	}

	CreateNewMap( m_currentMapName );
	if ( !needPlayCutscene )
	{
		m_currentMap->m_cutscenePlayer.m_isPlayingCutscene = false;
		m_currentMap->m_cutscenePlayer.m_currentCutscene = nullptr;
	}
	ClearDebugRendering();
	m_isJustWinCurrentLevel = false;
}

void World::EnterMap( const std::string& mapName )
{
	m_currentMapName = mapName;
	RestartCurrentMap();
	if( m_currentMap && m_currentMap->m_mapDef->m_beforeLevelCutscene )
	{
		m_currentMap->PlayCutscene( m_currentMap->m_mapDef->m_beforeLevelCutscene );
	}
}

void World::NextMap()
{
	std::string nextLevel = m_currentMap->m_mapDef->m_nextLevelName;
	if ( nextLevel == "" )
	{
		g_gameState = MAIN_MENU;
	}
	else
	{
		EnterMap( nextLevel );
	}
}

bool World::IsPlayerControllable() const
{
	return !m_isJustWinCurrentLevel && !m_isExitAlertShowing;
}

COMMAND( enter_map, "Enter map by name.", "name" )
{
	UNUSED( args );
	std::string mapName = args.GetValue( "name", "" );
	if ( "" == mapName )
	{
		return;
	}
	g_theGame->m_world->EnterMap( mapName );
}

COMMAND( next_map, "Go to next map.", "" )
{
	UNUSED( args );
	g_theGame->m_world->NextMap();
}