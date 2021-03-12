#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/World.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/CutsceneDefinition.hpp"
#include "Game/Entity/Entity.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/TextureCube.hpp"
#include "Engine/Core/ParticleSystem.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Math/AABB3.hpp"

extern EventSystem* g_theEventSystem;
Game* g_theGame = nullptr;
BitmapFont* g_theFont = nullptr;

Game::Game()
{
	m_clock = new Clock( Clock::GetMaster() );
}

Game::~Game()
{
}

void Game::Startup()
{
	m_worldCamera = new Camera( Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	m_worldCamera->SetProjectionOrthographic( WORLD_SIZE_Y );
	m_worldCamera->m_clearColor = Rgba8( 0, 0, 5 );
	m_worldCamera->InitialUBO( g_theRenderer );

	m_devConsoleCamera = new Camera( Vec2( 100.f, 50.f ) );
	m_devConsoleCamera->SetProjectionOrthographic( 50.f );
	m_devConsoleCamera->SetClearMode( CLEAR_NONE );
	m_devConsoleCamera->InitialUBO( g_theRenderer );
	g_theConsole->SetCamera(m_devConsoleCamera);

	ShaderState::LoadDefinitions( g_theRenderer, "Data/Definitions/ShaderStateDefs.xml" );
	Material::LoadDefinitions( g_theRenderer, "Data/Definitions/MaterialDefs.xml" );
	CutsceneDefinition::LoadDefinitions( "Data/Definitions/Cutscenes/" );
	ActorDefinition::LoadDefinitions( "Data/Definitions/ActorDefs.xml" );
	MapDefinition::LoadDefinitions( g_theRenderer, "Data/Definitions/MapDefs.xml" );

	g_theFont = g_theRenderer->CreateOrGetBitmapFont( "Data/Fonts/MyFixedFont" ); // NO FILE EXTENSION!
	g_theEventSystem->FireEvent( "megumin" );

	m_world = new World( this );
	//m_world->EnterMap( g_gameConfigBlackboard.GetValue( "startMap", "" ) );

	SpriteSheet* spriteSheet = new SpriteSheet( *g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/mainUIexport_oUI_4x2.png" ), IntVec2( 4, 2 ) );
	m_bgAnim = new SpriteAnimDefinition( *spriteSheet, 0, 7, 0.5f, SpriteAnimPlaybackType::LOOP );
	m_volume = g_gameConfigBlackboard.GetValue( "soundVolume", 1.f );
	LoadAllAudio( "Data/Audio" );
	m_currentAudio = g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/Menu_Apropos.mp3" ), true, m_volume );

	m_particleSystem = new ParticleSystem();
	Emitter* e = m_particleSystem->CreateEmitter();
	Emitter* e2 = m_particleSystem->CreateEmitter();
	Emitter* e3 = m_particleSystem->CreateEmitter();

	Vec2 cameraBottomLeft = m_worldCamera->GetOrthoBottomLeft();
	Vec2 cameraTopRight = m_worldCamera->GetOrthoTopRight();
	e->m_minOrientation = 35.f;
	e->m_maxOrientation = 55.f;
	e->m_position = Vec3( cameraBottomLeft.x, cameraBottomLeft.y - 1.f, 0.f );
	e->m_spawnOffsetMin = Vec3( -10.f, 0.f, 0.f );
	e->m_spawnOffsetMax = Vec3( cameraTopRight.x - cameraBottomLeft.x, 0.f, 0.f );
	e->m_maxAge = 4.f;
	e->m_interval = 0.1f;
	e->m_color = Rgba8( 240, 55, 31 );
	e->m_scale = Vec3( 0.5f, 0.5f, 1.f );
	e->m_texture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/menu_particle.png" );

	e2->m_minOrientation = 35.f;
	e2->m_maxOrientation = 55.f;
	e2->m_position = Vec3( cameraBottomLeft.x, cameraBottomLeft.y - 1.f, 0.f );
	e2->m_spawnOffsetMin = Vec3( -10.f, 0.f, 0.f );
	e2->m_spawnOffsetMax = Vec3( cameraTopRight.x - cameraBottomLeft.x, 0.f, 0.f );
	e2->m_maxAge = 4.f;
	e2->m_interval = 0.3f;
	e2->m_hasFadeOut = true;
	e2->m_color = Rgba8( 240, 55, 31 );
	e2->m_scale = Vec3( 0.3f, 0.3f, 1.f );
	e2->m_texture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/menu_particle.png" );

	e3->m_minOrientation = 70.f;
	e3->m_maxOrientation = 90.f;
	e3->m_position = Vec3( cameraBottomLeft.x, cameraBottomLeft.y - 1.f, 0.f );
	e3->m_spawnOffsetMin = Vec3( 0.f, 0.f, 0.f );
	e3->m_spawnOffsetMax = Vec3( cameraTopRight.x - cameraBottomLeft.x, 0.f, 0.f );
	e3->m_maxAge = 2.f;
	e3->m_interval = 0.5f;
	e3->m_velocity = 3.f;
	e3->m_hasFadeOut = true;
	e3->m_color = Rgba8( 240, 55, 31 );
	e3->m_scale = Vec3( 0.3f, 0.3f, 1.f );
	e3->m_texture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/menu_particle2.png" );
}

void Game::Update( float deltaSeconds )
{
	UpdateBasicInput();
	if ( g_theConsole->IsOpen() )
	{
		return;
	}

	switch( g_gameState )
	{
	case MAIN_MENU:		UpdateMainMenu( deltaSeconds );			break;
	case PLAY_MODE:		UpdatePlayingLevel( deltaSeconds );		break;
	case LEVEL_SELECT:	UpdateLevelSelect( deltaSeconds );		break;
	case SETTINGS:		UpdateSettings( deltaSeconds );			break;
	case CONTROLS:		UpdateControls( deltaSeconds );			break;
	default: ERROR_AND_DIE("UNKNOWN GAME STATE.");
	}
}

void Game::UpdatePlayingLevel( float deltaSeconds )
{
	if( m_world )
	{
		m_world->Update( deltaSeconds );
		m_world->EndFrame();

		if( g_theInput->WasKeyJustPressed( 'R' ) && !m_world->m_currentMap->m_cutscenePlayer.IsPlayingCutscene() )
		{
			g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/TestSound.mp3" ), false, m_volume );
			m_world->RestartCurrentMap( false );
		}
	}
}

void Game::UpdateLevelSelect( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	Strings mapNames = MapDefinition::GetAllMapNames();
	int mapLength = (int) mapNames.size();

	if( g_theInput->WasKeyJustPressed( VK_RIGHT ) || g_theInput->WasKeyJustPressed( 'D' ) )
	{
		++m_uiButtonIndex;
		m_uiButtonIndex = Clamp( m_uiButtonIndex, 0, mapLength - 1 );
		g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/button_menu_highlight_01.mp3" ), false, m_volume );
	}
	if( g_theInput->WasKeyJustPressed( VK_LEFT ) || g_theInput->WasKeyJustPressed( 'A' ) )
	{
		--m_uiButtonIndex;
		m_uiButtonIndex = Clamp( m_uiButtonIndex, 0, mapLength - 1 );
		g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/button_menu_highlight_01.mp3" ), false, m_volume );
	}
	if( g_theInput->WasKeyJustPressed( VK_RETURN ) )
	{
		if ( m_uiButtonIndex == mapLength )
		{
			ChangeGameState( MAIN_MENU );
		}
		else
		{
			m_world->EnterMap( mapNames[m_uiButtonIndex] );
			ChangeGameState( PLAY_MODE );
		}
		g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/button_menu_confirm_01.mp3" ), false, m_volume );
	}
}

void Game::UpdateSettings( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	if ( g_theInput->IsKeyPressed( VK_LEFT ) || g_theInput->IsKeyPressed( 'A' ) )
	{
		m_volume -= deltaSeconds;
		m_volume = Clamp( m_volume, 0.f, 1.f );
		g_theAudio->SetSoundPlaybackVolume( m_currentAudio, m_volume );
	}
	if ( g_theInput->IsKeyPressed( VK_RIGHT ) || g_theInput->IsKeyPressed( 'D' ) )
	{
		m_volume += deltaSeconds;
		m_volume = Clamp( m_volume, 0.f, 1.f );
		g_theAudio->SetSoundPlaybackVolume( m_currentAudio, m_volume );
	}
}

void Game::UpdateControls( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}

void Game::UpdateMainMenu( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	if ( g_theInput->WasKeyJustPressed( VK_DOWN ) || g_theInput->WasKeyJustPressed( 'S' ) )
	{
		++m_uiButtonIndex;
		m_uiButtonIndex = Clamp( m_uiButtonIndex, 0, 4 );
		g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/button_menu_highlight_01.mp3" ), false, m_volume );
	}
	if ( g_theInput->WasKeyJustPressed( VK_UP ) || g_theInput->WasKeyJustPressed( 'W' ) )
	{
		--m_uiButtonIndex;
		m_uiButtonIndex = Clamp( m_uiButtonIndex, 0, 4 );
		g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/button_menu_highlight_01.mp3" ), false, m_volume );
	}

	if( g_theInput->WasKeyJustPressed( VK_RETURN ) )
	{
		switch( m_uiButtonIndex )
		{
		case 0:
			m_world->EnterMap( g_gameConfigBlackboard.GetValue( "startMap", "" ) );
			ChangeGameState( PLAY_MODE );
			break;
		case 1: ChangeGameState( LEVEL_SELECT );	break;
		case 2: ChangeGameState( SETTINGS );		break;
		case 3: ChangeGameState( CONTROLS );		break;
		case 4: g_theEventSystem->FireEvent("quit"); break;
		default: break;
		}
		g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/button_menu_confirm_01.mp3" ), false, m_volume );
	}

	m_particleSystem->Update(deltaSeconds);
}

void Game::Shutdown()
{
	m_worldCamera->ClearupUBO();
	m_devConsoleCamera->ClearupUBO();

	uint numObjects = (uint)m_gameObjects.size();
	for( uint i = 0; i < numObjects; ++i ) {
		GameObject* go = m_gameObjects[i];
		delete go;
		go = nullptr;
	}

	// Save volume value to game config
	XmlDocument xmlDocument;
	xmlDocument.LoadFile( "Data/GameConfig.xml" );
	if ( !xmlDocument.Error() )
	{
		XmlElement* rootElement = xmlDocument.RootElement();

		rootElement->SetAttribute( "soundVolume", m_volume );
		xmlDocument.SaveFile( "Data/GameConfig.xml" );
	}
}

void Game::Render() const
{
	g_theRenderer->BeginCamera( *m_worldCamera );

	switch( g_gameState )
	{
	case MAIN_MENU:		RenderMainMenu();			break;
	case PLAY_MODE:		RenderPlayingLevel();		break;
	case LEVEL_SELECT:	RenderLevelSelect();		break;
	case SETTINGS:		RenderSettings();			break;
	case CONTROLS:		RenderControls();			break;
	default: ERROR_AND_DIE( "UNKNOWN GAME STATE." );
	}


	g_theRenderer->BindShader( nullptr );
	g_theRenderer->BindSampler( nullptr );
	DebugRenderWorldToCamera( m_worldCamera );

	g_theRenderer->EndCamera( *m_worldCamera );
}

void Game::RenderMainMenu() const
{
	AABB2 screenBounds = DebugGetScreenBounds();

	AABB2 bgBounds = AABB2( Vec2::ZERO, Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	bgBounds.Translate( -Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) * 0.5f );
	std::vector<Vertex_PCU> vertices;
	AppendQuad( vertices, Vec3( bgBounds.mins, 0.f ), Vec3( bgBounds.maxs.x, bgBounds.mins.y, 0.f ),
		Vec3( bgBounds.mins.x, bgBounds.maxs.y, 0.f ), Vec3( bgBounds.maxs, 0.f ), AABB2::ZERO_TO_ONE, Rgba8::WHITE );
	Texture* bgTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/UI/background.png" );
	g_theRenderer->BindTexture( bgTex );
	g_theRenderer->DrawVertexArray( vertices );

	m_particleSystem->Render();

	AABB2 titleBounds = AABB2( Vec2::ZERO, Vec2( 1200.f, 120.f ) );
	titleBounds.TranslateAtPivot( screenBounds, Vec4( 0.5f, 0.5f, 0.f, 300.f ), ALIGN_CENTERED );
	Texture* titleTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/UI/title.png" );
	DebugAddScreenTexturedQuad( titleBounds, titleTex );

	std::vector<std::string> mainMenuOptions;
	mainMenuOptions.push_back( "START GAME" );
	mainMenuOptions.push_back( "LEVEL SELECT" );
	mainMenuOptions.push_back( "SETTINGS" );
	mainMenuOptions.push_back( "CONTROLS" );
	mainMenuOptions.push_back( "EXIT" );
	for ( int i = 0; i < (int) mainMenuOptions.size(); ++i )
	{
		float offsetPosY = -100.f * i;
		if ( m_uiButtonIndex == i )
		{
			AABB2 selectFrameBounds = AABB2( Vec2::ZERO, Vec2( 1100.f, 110.f ) );
			Texture* selectBtnTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/UI/mainmenu_selectbutton.png" );
			AABB2 bounds = selectFrameBounds.GetTranslateAtPivot( screenBounds, Vec4( 0.5f, 0.5f, 0.f, offsetPosY ), ALIGN_CENTERED );
			DebugAddScreenTexturedQuad( bounds, selectBtnTex, Rgba8( 230, 76, 81 ) );
			DebugAddScreenTextf( Vec4( 0.5f, 0.5f, 0.f, offsetPosY ), ALIGN_CENTERED, 30.f, Rgba8::WHITE, 0.f, mainMenuOptions[i].c_str() );
		}
		else
		{
			AABB2 unselectFrameBounds = AABB2( Vec2::ZERO, Vec2( 1000.f, 100.f ) );
			Texture* unselectBtnTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/UI/mainmenu_button.png" );
			AABB2 bounds = unselectFrameBounds.GetTranslateAtPivot( DebugGetScreenBounds(), Vec4( 0.5f, 0.5f, 0.f, offsetPosY ), ALIGN_CENTERED );
			DebugAddScreenTexturedQuad( bounds, unselectBtnTex, Rgba8( 102, 61, 73 ) );
			DebugAddScreenTextf( Vec4( 0.5f, 0.5f, 0.f, offsetPosY ), ALIGN_CENTERED, 30.f, Rgba8( 156, 135, 146 ), 0.f, mainMenuOptions[i].c_str() );
		}
	}
}

void Game::RenderPlayingLevel() const
{
	if( m_world )
		m_world->Render();

	if( g_isDebugMode )
	{
		Vec2 mousePos = g_theInput->GetMouseNormalizedClientPos();
		mousePos = m_worldCamera->ClientToWorldPosition( mousePos, 0.f );
		DebugAddMessage( 0.f, Rgba8::WHITE, "mouse = (x:%.2f, y:%.2f)", mousePos.x, mousePos.y );
	}
}

void Game::RenderLevelSelect() const
{
	DebugAddScreenTextf( Vec4( 0.5f, 0.5f, 0.f, 300.f ), ALIGN_CENTERED, 100.f, Rgba8::WHITE, 0.f, "LEVEL SELECT" );

	const std::vector<MapDefinition*> mapDefs = MapDefinition::GetAllMapDefs();
	int mapLength = (int) mapDefs.size();
	Texture* frameTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/UI/W_chapter1.png" );
	Texture* upFrameTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/UI/W_chapterSelect.png" );
	Texture* downFrameTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/UI/W_chapterSelect2.png" );
	AABB2 screenBounds = DebugGetScreenBounds();
	AABB2 frameBounds = AABB2( Vec2::ZERO, Vec2( 130.f, 100.f ) );
	AABB2 outFrameBounds = AABB2( Vec2::ZERO, Vec2( 1438.f, 54.f ) );
	DebugAddScreenTexturedQuad( outFrameBounds.GetTranslateAtPivot( screenBounds, Vec4( 0.5f, 0.5f, 0.f, -65.f ), ALIGN_CENTERED ), upFrameTex, Rgba8( 230, 76, 81 ) );
	DebugAddScreenTexturedQuad( outFrameBounds.GetTranslateAtPivot( screenBounds, Vec4( 0.5f, 0.5f, 0.f, 65.f ), ALIGN_CENTERED ), downFrameTex, Rgba8( 230, 76, 81 ) );
	float offsetX = 230.f;
	for ( int i = 0; i < mapLength; ++i )
	{
		float offsetPosX = 150.f * i - offsetX;

		if( m_uiButtonIndex == i )
		{
			std::string displayStr = Stringf( "%s", mapDefs[i]->m_levelName.c_str() );
			DebugAddScreenTextf( Vec4( 0.5f, 0.5f, offsetPosX, 0.f ), ALIGN_CENTERED, 30.f, Rgba8::WHITE, 0.f, displayStr.c_str() );
			DebugAddScreenTexturedQuad( frameBounds.GetTranslateAtPivot( screenBounds, Vec4( 0.5f, 0.5f, offsetPosX, 0.f ), ALIGN_CENTERED ), frameTex, Rgba8::WHITE );
			DebugAddScreenTexturedQuad( frameBounds.GetTranslateAtPivot( screenBounds, Vec4( 0.5f, 0.5f, offsetPosX, 0.f ), ALIGN_CENTERED ), nullptr, Rgba8( 255, 255, 255, 50 ) );
		}
		else
		{
			std::string displayStr = Stringf( "%s", mapDefs[i]->m_levelName.c_str() );
			DebugAddScreenTextf( Vec4( 0.5f, 0.5f, offsetPosX, 0.f ), ALIGN_CENTERED, 30.f, Rgba8( 255, 255, 255, 100 ), 0.f, displayStr.c_str() );
			DebugAddScreenTexturedQuad( frameBounds.GetTranslateAtPivot( screenBounds, Vec4( 0.5f, 0.5f, offsetPosX, 0.f ), ALIGN_CENTERED ), frameTex, Rgba8( 255, 255, 255, 100 ) );
		}
	}
	std::string levelStr = Stringf( "%s - %s", mapDefs[m_uiButtonIndex]->m_levelName.c_str(), mapDefs[m_uiButtonIndex]->m_name.c_str() );
	DebugAddScreenTextf( Vec4( 0.5f, 0.5f, 0.f, -120.f ), ALIGN_CENTERED, 30.f, Rgba8::WHITE, 0.f, levelStr.c_str() );

	AABB2 successBounds = AABB2( Vec2::ZERO, Vec2( 500.f, 500.f ) );
	successBounds.TranslateAtPivot( screenBounds, Vec4( 0.f, 0.f, 0.f, 0.f ), ALIGN_BOTTOM_LEFT );
	float seconds = (float)m_clock->GetTotalTime();
	const SpriteDefinition& sd = m_bgAnim->GetSpriteDefAtTime( seconds );
	const Texture& successTex = sd.GetTexture();
	Vec2 uvAtMins, uvAtMaxs;
	sd.GetUVs( uvAtMins, uvAtMaxs );
	DebugAddScreenTexturedQuad( successBounds, &successTex, AABB2( uvAtMins, uvAtMaxs ), Rgba8::WHITE, 0.f );
}

void Game::RenderSettings() const
{
	DebugAddScreenTextf( Vec4( 0.5f, 0.5f, 0.f, 300.f ), ALIGN_CENTERED, 100.f, Rgba8::WHITE, 0.f, "SETTINGS" );

	AABB2 selectFrameBounds = AABB2( Vec2::ZERO, Vec2( 500.f, 110.f ) );
	AABB2 bounds = selectFrameBounds.GetTranslateAtPivot( DebugGetScreenBounds(), Vec4( 0.5f, 0.5f, 0.f, -100.f ), ALIGN_MIDDLE_LEFT );
	DebugAddScreenTexturedQuad( bounds, nullptr, Rgba8( 255, 255, 255, 100 ) );
	DebugAddScreenTexturedQuad( bounds.CarveBoxOffLeft( m_volume ), nullptr, Rgba8( 230, 76, 81 ) );
	DebugAddScreenTextf( Vec4( 0.5f, 0.5f, -100.f, -100.f ), ALIGN_MIDDLE_RIGHT, 30.f, Rgba8::WHITE, 0.f, "SOUND VOLUME" );
}

void Game::RenderControls() const
{
	//DebugAddScreenTextf( Vec4( 0.5f, 0.5f, 0.f, 300.f ), ALIGN_CENTERED, 100.f, Rgba8::WHITE, 0.f, "CONTROLS" );
	Texture* bgTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/UI/control-background.png" );
	DebugAddScreenTexturedQuad( DebugGetScreenBounds(), bgTex, Rgba8::WHITE, 0.f );
}

void Game::RenderDebug() const
{
	DebugRenderScreenTo( g_theRenderer );

	g_theRenderer->BeginCamera( *m_devConsoleCamera );

	g_theConsole->m_backgroundAnimBox = AABB2( m_devConsoleCamera->GetOrthoTopRight() - Vec2( 33.f, 62.f ), m_devConsoleCamera->GetOrthoTopRight() );
	g_theConsole->Render( *g_theRenderer, *m_devConsoleCamera, 1.f, g_theFont );

	g_theRenderer->EndCamera( *m_devConsoleCamera );
}

void Game::UpdateBasicInput()
{
	//if( g_theInput->WasKeyJustPressed( VK_OEM_3 ) )
	//{
	//	g_theConsole->ToggleOpenFull();
	//	//g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/Pause.mp3" ) );
	//}
	//if( g_theInput->WasKeyJustPressed( VK_F1 ) )
	//{
	//	g_isDebugMode = !g_isDebugMode;
	//}
	if( g_theInput->WasKeyJustPressed( VK_ESCAPE ) )
	{
		if( g_theConsole->IsOpen() )
		{
			g_theConsole->Close();
		}
		else
		{
			switch( g_gameState )
			{
			case MAIN_MENU:		g_theEventSystem->FireEvent( "quit" );	break;
			case LEVEL_SELECT:	ChangeGameState( MAIN_MENU );			break;
			case PLAY_MODE:												break;
			case SETTINGS:		ChangeGameState( MAIN_MENU );			break;
			case CONTROLS:		ChangeGameState( MAIN_MENU );			break;
			default:			g_theEventSystem->FireEvent( "quit" );	break;
			}
			g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/button_menu_back_01.mp3" ), false, m_volume );
		}
	}
}

void Game::ChangeGameState( eGameState gameState )
{
	if ( g_gameState == PLAY_MODE && gameState == MAIN_MENU )
	{
		g_theAudio->StopSound( m_currentAudio );
		m_currentAudio = g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/Menu_Apropos.mp3" ), true, m_volume );
	}
	if ( ( g_gameState == MAIN_MENU || g_gameState == LEVEL_SELECT ) && gameState == PLAY_MODE )
	{
		g_theAudio->StopSound( m_currentAudio );
		m_currentAudio = g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/Vitality.mp3" ), true, m_volume );
	}
	g_gameState = gameState;
	m_uiButtonIndex = 0;
}

Map* Game::GetCurrentMap() const
{
	return m_world->GetCurrentMap();
}

void Game::LoadAllAudio( const char* folderPath ) const
{
	if ( !folderPath )
	{
		return;
	}

	Strings audios = GetFileNamesInFolder( folderPath, "*.mp3" );
	std::string folder(folderPath);
	for ( std::string a : audios )
	{
		std::string path = folder + "/" + a;
		g_theAudio->CreateOrGetSound( path );
	}
}

COMMAND( megumin, "Megumin will appear on console.", "" )
{
	UNUSED(args);
	SpriteSheet* spriteSheet = new SpriteSheet( *g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Megumin.png" ), IntVec2( 6, 2 ) );
	g_theConsole->m_backgroundAnim = new SpriteAnimDefinition( *spriteSheet, 0, 11, 1.f, SpriteAnimPlaybackType::LOOP );
}

COMMAND( dance, "Megumin will start dancing.", "" )
{
	UNUSED( args );
	SpriteSheet* spriteSheet = new SpriteSheet( *g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Megumin_Dancing.png" ), IntVec2( 6, 3 ) );
	g_theConsole->m_backgroundAnim = new SpriteAnimDefinition( *spriteSheet, 0, 16, 2.f, SpriteAnimPlaybackType::LOOP );
}