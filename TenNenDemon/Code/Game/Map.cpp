#include "Map.hpp"
#include "World.hpp"
#include "GameCommon.hpp"
#include "Game.hpp"
#include "Game/World.hpp"
#include "Game/Entity/Actor.hpp"
#include "Game/Entity/Player.hpp"
#include "Game/Entity/Fireball.hpp"
#include "Game/Entity/Lava.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/EnvironmentEntityDefinition.hpp"
#include "Game/CutsceneDefinition.hpp"
#include "Game/Entity/EnvironmentEntity.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/ParticleSystem.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Timer.hpp"

Map::Map( Game* game, const std::string& name )
	:m_game( game ),
	m_name( name )
{
	m_mapClock = new Clock();
	m_successTimer = new Timer( m_mapClock );
	m_particleSystem = new ParticleSystem();
	m_mapDef = MapDefinition::GetDefinitions( name );
	m_levelName = m_mapDef->m_levelName;
	m_tileDimensions = m_mapDef->m_dimensions;
	m_abilityLimitNumber = m_mapDef->m_startAbilityLimitNumber;

	PopulateTiles();
	SpawnNewEntity( "Player", GetTileCenterPosition( m_mapDef->m_playerStartIndex ), ENTITY_TYPE_PLAYER );
	SpawnNewEntity( "Pet", GetTileCenterPosition( m_mapDef->m_petStartIndex ), ENTITY_TYPE_PET );
	for ( int i = 0; i < (int) m_mapDef->m_environmentEntities.size(); i++ )
	{
		EnvironmentEntityDefinition* def = m_mapDef->m_environmentEntities[i];
		SpawnNewEnvironmentEntity( GetTileCenterPosition( def->m_startIndex ), def, def->m_entityType );
	}

	SpriteSheet* spriteSheet = new SpriteSheet( *g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Cutscenes/booper_spritesheet_7x4.png" ), IntVec2( 7, 4 ) );
	m_cutscenePlayer.m_booperAnim = new SpriteAnimDefinition( *spriteSheet, 0, 14, 1.f, SpriteAnimPlaybackType::LOOP );
	SpriteSheet* spriteSheet2 = new SpriteSheet( *g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/UI/success_spritesheet_2x8.png" ), IntVec2( 2, 8 ) );
	m_successAnim = new SpriteAnimDefinition( *spriteSheet2, 0, 7, 0.8f, SpriteAnimPlaybackType::ONCE );

	m_playerDust = m_particleSystem->CreateEmitter();
	m_playerDust->m_color = Rgba8( 66, 26, 2 );
	m_playerDust->m_hasFadeOut = true;
	m_playerDust->m_minOrientation = 45.f;
	m_playerDust->m_maxOrientation = 135.f;
	m_playerDust->m_scale = Vec3( 0.1f, 0.1f, 1.f );
	m_playerDust->m_velocity = 0.5f;
	m_playerDust->m_interval = 0.005f;
}

Map::~Map()
{
}

Entity* Map::SpawnNewEntity( const std::string& name, const Vec2& pos, eEntityType entityType )
{
	Entity* newEntity = nullptr;
	switch( entityType )
	{
	case ENTITY_TYPE_PLAYER:	newEntity = new Player( m_game, this, pos, name );						break;
	case ENTITY_TYPE_FIREBALL:	newEntity = new Fireball( m_game, this, pos, name );					break;
	case ENTITY_TYPE_PET:		newEntity = new Actor( m_game, this, pos, name, ENTITY_TYPE_PET );		break;
	default:					ERROR_AND_DIE( "UNKNOWN TYPE OF ENTITY!" );								break;
	}
	AddEntityToMap( newEntity );
	return newEntity;
}

void Map::SpawnNewEnvironmentEntity( const Vec2& pos, EnvironmentEntityDefinition* def, eEntityType entityType )
{
	if ( entityType == ENTITY_TYPE_LAVA )
	{
		Entity* newEntity = new Lava( m_game, this, pos, def );
		AddEntityToMap( newEntity );
	}
	else
	{
		Entity* newEntity = new EnvironmentEntity( m_game, this, pos, def );
		AddEntityToMap( newEntity );
	}
}

void Map::UseFireballAbility( const IntVec2& direction )
{
	if ( m_abilityLimitNumber <= 0 )
	{
		g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/button_menu_back_01.mp3" ), false, g_theGame->m_volume );
		return;
	}
	Entity* player = m_entityListsByType[ENTITY_TYPE_PLAYER][0];
	IntVec2 spawnIdx = player->m_currentPositionIndex;
	if ( IsOutOfBounds( spawnIdx ) )
	{
		return;
	}

	g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/fireball.mp3" ), false, g_theGame->m_volume );
	Fireball* newFireball = dynamic_cast<Fireball*>( SpawnNewEntity( "FireBall", GetTileCenterPosition( spawnIdx ), ENTITY_TYPE_FIREBALL ) );
	newFireball->m_currentPositionIndex = spawnIdx;
	newFireball->SetMovingDirection( direction );
	m_abilityLimitNumber--;
}

void Map::PlayCutscene( const CutsceneDefinition* cutsceneDef, const std::string& afterPlayingEventName )
{
	g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/dialogue_start_01.mp3" ), false, g_theGame->m_volume );
	m_cutscenePlayer.m_isPlayingCutscene = true;
	m_cutscenePlayer.m_currentCutscene = cutsceneDef;
	m_cutscenePlayer.m_afterPlayingEventName = afterPlayingEventName;
}

bool Map::IsEntityAlive( const Entity* entity ) const
{
	if ( entity->IsDead() )
	{
		return false;
	}
	return true;
}

bool Map::IsOutOfBounds( int tileX, int tileY )
{
	if( tileX < 0 || tileY < 0 ||
		m_tileDimensions.x - 1 < tileX || m_tileDimensions.y - 1 < tileY )
	{
		return true;
	}
	return false;
}

bool Map::IsOutOfBounds( const IntVec2& tileCoords ) const
{
	if( tileCoords.x < 0 || tileCoords.y < 0 || 
		m_tileDimensions.x - 1 < tileCoords.x || m_tileDimensions.y - 1 < tileCoords.y )
	{
		return true;
	}
	return false;
}

int Map::GetTileIndexForTileCoords( const IntVec2& tileCoords ) const
{
	return tileCoords.x + ( m_tileDimensions.x * tileCoords.y );
}

int Map::GetTileIndexForTileCoords( int tileX, int tileY ) const
{
	return tileX + ( m_tileDimensions.x * tileY );
}

IntVec2 Map::GetTileCoordsForWorldPos( const Vec2& worldPos ) const
{
	int posX = RoundDownToInt( worldPos.x );
	int posY = RoundDownToInt( worldPos.y );
	return IntVec2( posX, posY );
}

void Map::AddEntityToMap( Entity* entity )
{
	for ( int listIndex = 0;listIndex < (int) m_entityListsByType[ entity->m_entityType ].size();listIndex++ )
	{
		if ( m_entityListsByType[ entity->m_entityType ][listIndex] == nullptr )
		{
			m_entityListsByType[ entity->m_entityType ][listIndex] = entity;
			return;
		}
	}
	m_entityListsByType[ entity->m_entityType ].push_back( entity );
}

void Map::PopulateTiles()
{
	int numTiles = m_tileDimensions.x * m_tileDimensions.y;

	for( int tileIndex = 0; tileIndex < numTiles; tileIndex++ )
	{
		int tileX = tileIndex % m_tileDimensions.x;
		int tileY = tileIndex / m_tileDimensions.x;
		m_tiles.push_back( Tile( tileX, tileY, m_mapDef->m_mapStartPoint ) );
	}
}

void Map::CheckIsWinning()
{
	Entity* player = m_entityListsByType[ENTITY_TYPE_PLAYER][0];
	Entity* pet = m_entityListsByType[ENTITY_TYPE_PET][0];
	if( pet->m_currentPositionIndex == player->m_targetPositionIndex + IntVec2( 1, 0 ) )
	{
		m_game->m_world->WinCurrentLevel();
	}
	else if( pet->m_currentPositionIndex == player->m_targetPositionIndex + IntVec2( -1, 0 ) )
	{
		m_game->m_world->WinCurrentLevel();
	}
	else if( pet->m_currentPositionIndex == player->m_targetPositionIndex + IntVec2( 0, 1 ) )
	{
		m_game->m_world->WinCurrentLevel();
	}
	else if( pet->m_currentPositionIndex == player->m_targetPositionIndex + IntVec2( 0, -1 ) )
	{
		m_game->m_world->WinCurrentLevel();
	}
}

void Map::Render() const
{
	if( m_cutscenePlayer.m_isPlayingCutscene )
	{
		RenderCutscene();
	}
	else
	{
		RenderBackground();
		RenderEntities();
		m_particleSystem->Render();
		RenderUI();
	}

	DebugModeRender();
}

void Map::RenderCutscene() const
{
	AABB2 screenBounds = DebugGetScreenBounds();
	AABB2 backgroundBounds = AABB2( Vec2::ZERO, Vec2( 1960.f, 544.f ) );
	backgroundBounds.TranslateAtPivot( screenBounds, Vec4( 0.5f, 0.4f, 0.f, 0.f ), ALIGN_BOTTOM_MIDDLE );
	Texture* bgTex = m_cutscenePlayer.m_currentCutscene->m_lines[m_cutscenePlayer.m_currentLineIndex]->m_background;
	if ( bgTex )
	{
		DebugAddScreenTexturedQuad( backgroundBounds, bgTex, Rgba8::WHITE );
	}

	AABB2 characterBounds = AABB2( Vec2::ZERO, Vec2( 500.f, 544.f ) );
	characterBounds.TranslateAtPivot( screenBounds, Vec4( 0.5f, 0.4f, 0.f, 0.f ), ALIGN_BOTTOM_MIDDLE );
	Texture* characterTex = m_cutscenePlayer.m_currentCutscene->m_lines[m_cutscenePlayer.m_currentLineIndex]->m_characterImage;
	if ( characterTex )
	{
		DebugAddScreenTexturedQuad( characterBounds, characterTex, Rgba8::WHITE );
	}

	const std::string& nameStr = m_cutscenePlayer.m_currentCutscene->m_lines[m_cutscenePlayer.m_currentLineIndex]->m_characterName;
	DebugAddScreenTextf( Vec4( 0.5f, 0.5f, 0.f, -170.f ), ALIGN_CENTERED, 30.f, Rgba8( 237, 71, 82 ), 0.f, nameStr.c_str() );
	const std::string& lineStr = m_cutscenePlayer.m_line;
	DebugAddScreenTextf( Vec4( 0.5f, 0.5f, 0.f, -230.f ), ALIGN_CENTERED, 30.f, Rgba8::WHITE, 0.f, lineStr.c_str() );

	std::string line = m_cutscenePlayer.m_currentCutscene->m_lines[m_cutscenePlayer.m_currentLineIndex]->m_line;
	if( line.size() == m_cutscenePlayer.m_lineStrIndex - 1 )
	{
		AABB2 booperBounds = AABB2( Vec2::ZERO, Vec2( 90.f, 40.f ) );
		booperBounds.TranslateAtPivot( screenBounds, Vec4( 0.5f, 0.5f, 0.f, -400.f ), ALIGN_CENTERED );
		float seconds = (float)m_mapClock->GetTotalElapsedSeconds();
		const SpriteDefinition& sd = m_cutscenePlayer.m_booperAnim->GetSpriteDefAtTime( seconds );
		const Texture& booperTex = sd.GetTexture();
		Vec2 uvAtMins, uvAtMaxs;
		sd.GetUVs( uvAtMins, uvAtMaxs );
		DebugAddScreenTexturedQuad( booperBounds, &booperTex, AABB2( uvAtMins, uvAtMaxs ), Rgba8( 237, 71, 82 ), 0.f );
	}
}

void Map::RenderBackground() const
{
	Camera* camera = g_theGame->GetWorldCamera();
	AABB2 backgroundAABB = AABB2( camera->GetOrthoMin().GetXY(), camera->GetOrthoMax().GetXY() );
	g_theRenderer->BindTexture( m_mapDef->m_backgroundImage );
	g_theRenderer->DrawAABB2D(backgroundAABB, AABB2::ZERO_TO_ONE, Rgba8::WHITE);
}

void Map::RenderUI() const
{
	AABB2 uiBounds = DebugGetScreenBounds();
	Texture* mainUITex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/UI/mainUI.png" );
	DebugAddScreenTexturedQuad( uiBounds, mainUITex );
	if ( GetAbilityLimitNumber() == 0 )
	{
		DebugAddScreenTextf( Vec4( 0.f, 0.f, 210.f, 250.f ), ALIGN_CENTERED, 100.f, Rgba8::RED, 0.f, "%i", GetAbilityLimitNumber() );
	}
	else
	{
		DebugAddScreenTextf( Vec4( 0.f, 0.f, 210.f, 250.f ), ALIGN_CENTERED, 100.f, Rgba8::WHITE, 0.f, "%i", GetAbilityLimitNumber() );
	}
	DebugAddScreenTextf( Vec4( 1.f, 0.f, -210.f, 250.f ), ALIGN_CENTERED, 100.f, Rgba8::WHITE, 0.f, m_levelName.c_str() );
	DebugAddScreenTextf( Vec4( 0.5f, 0.f, 0.f, 0.f ), ALIGN_BOTTOM_MIDDLE, 30.f, Rgba8::WHITE, 0.f, "RESTART[R]" );

	if ( m_game->m_world->m_isJustWinCurrentLevel )
	{
		AABB2 screenBounds = DebugGetScreenBounds();
		AABB2 successBounds = AABB2( Vec2::ZERO, Vec2( 1100.f, 200.f ) );
		successBounds.TranslateAtPivot( screenBounds, Vec4( 0.5f, 0.5f, 0.f, 0.f ), ALIGN_CENTERED );
		float seconds = (float) m_successTimer->GetSecondsRemaining();
		const SpriteDefinition& sd = m_successAnim->GetSpriteDefAtTime( seconds );
		const Texture& successTex = sd.GetTexture();
		Texture* frameTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/UI/frame.png" );
		Vec2 uvAtMins, uvAtMaxs;
		sd.GetUVs( uvAtMins, uvAtMaxs );
		DebugAddScreenTexturedQuad( successBounds, &successTex, AABB2( uvAtMins, uvAtMaxs ), Rgba8( 255, 255, 255, 230 ), 0.f );
		DebugAddScreenTexturedQuad( successBounds, frameTex, AABB2::ZERO_TO_ONE, Rgba8( 255, 255, 255, 230 ), 0.f );
	}
}

void Map::RenderEntities() const
{
	for( int entityTypeIndex = 0; entityTypeIndex < NUM_ENTITY_TYPES; entityTypeIndex++ )
	{
		EntityList entityLists = m_entityListsByType[entityTypeIndex];
		for( int entityIndex = 0; entityIndex < (int) entityLists.size(); entityIndex++ )
		{
			if (entityLists[entityIndex])
			{
				entityLists[entityIndex]->Render();
			}
		}
	}
}

void Map::ClearDeadEntities()
{
	for( int entityTypeIndex = 0; entityTypeIndex < NUM_ENTITY_TYPES; entityTypeIndex++ )
	{
		EntityList& entityLists = m_entityListsByType[entityTypeIndex];
		for( int entityIndex = 0; entityIndex < (int)entityLists.size(); entityIndex++ )
		{
			if( entityLists[entityIndex] && entityLists[entityIndex]->m_isReadyToBeDeleted )
			{
				delete entityLists[entityIndex];
				entityLists[entityIndex] = nullptr;

				uint lastIndex = (uint)entityLists.size() - 1;
				entityLists[entityIndex] = entityLists[lastIndex];
				entityLists.pop_back();
				--entityIndex;
			}
		}
	}
}

void Map::DebugModeRender() const
{
	if ( !g_isDebugMode )
	{
		return;
	}

	if( (int)m_tiles.size() != 0 )
	{
		for( int tileIndex = 0; tileIndex < m_tiles.size(); tileIndex++ )
		{
			m_tiles[tileIndex].DebugModeRender();
		}
	}
}

bool Map::MovePlayerIfPossible( int directionX, int directionY )
{
	Entity* player = m_entityListsByType[ENTITY_TYPE_PLAYER][0];
	IntVec2 targetIndex = player->m_currentPositionIndex + IntVec2( directionX, directionY );
	if ( IsOutOfBounds( targetIndex ) )
	{
		return false;
	}

	for ( int i = 0; i < (int) m_entityListsByType[ENTITY_TYPE_BREAKABLE_OBJECT].size(); ++i )
	{
		Entity* breakableEntity = m_entityListsByType[ENTITY_TYPE_BREAKABLE_OBJECT][i];
		if ( breakableEntity->m_currentPositionIndex == targetIndex )
		{
			IntVec2 otherEntityTargetIndex = targetIndex + IntVec2( directionX, directionY );
			if ( MoveEnvironmentEntityIfPossible( breakableEntity, otherEntityTargetIndex.x, otherEntityTargetIndex.y ) )
			{
				player->Move( targetIndex.x, targetIndex.y );
				CheckIsWinning();
				return true;
			}
			return false;
		}
	}

	for ( int i = 0; i < (int) m_entityListsByType[ENTITY_TYPE_LAVA].size(); ++i )
	{
		Lava* lavaEntity = dynamic_cast<Lava*>(m_entityListsByType[ENTITY_TYPE_LAVA][i]);
		if ( lavaEntity->m_currentPositionIndex == targetIndex && !lavaEntity->IsFilled() )
		{
			return false;
		}
	}

	for( int i = 0; i < (int)m_entityListsByType[ENTITY_TYPE_UNBREAKABLE_OBJECT].size(); ++i )
	{
		Entity* unbreakableEntity = m_entityListsByType[ENTITY_TYPE_UNBREAKABLE_OBJECT][i];
		if( unbreakableEntity->m_currentPositionIndex == targetIndex )
		{
			return false;
		}
	}

	player->Move( targetIndex.x, targetIndex.y );
	CheckIsWinning();
	g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/character_move_01.mp3" ), false, g_theGame->m_volume );

	return true;
}

bool Map::MoveEnvironmentEntityIfPossible( Entity* environmentEntity, int tileX, int tileY )
{
	if( IsOutOfBounds( tileX, tileY ) )
	{
		return false;
	}

	for( int i = 0; i < (int)m_entityListsByType[ENTITY_TYPE_BREAKABLE_OBJECT].size(); ++i )
	{
		Entity* breakableEntity = m_entityListsByType[ENTITY_TYPE_BREAKABLE_OBJECT][i];
		if( breakableEntity->m_currentPositionIndex == IntVec2( tileX, tileY ) )
		{
			return false;
		}
	}

	for( int i = 0; i < (int)m_entityListsByType[ENTITY_TYPE_UNBREAKABLE_OBJECT].size(); ++i )
	{
		Entity* unbreakableEntity = m_entityListsByType[ENTITY_TYPE_UNBREAKABLE_OBJECT][i];
		if( unbreakableEntity->m_currentPositionIndex == IntVec2( tileX, tileY ) )
		{
			return false;
		}
	}

	for( int i = 0; i < (int)m_entityListsByType[ENTITY_TYPE_LAVA].size(); ++i )
	{
		Lava* lavaEntity = dynamic_cast<Lava*>(m_entityListsByType[ENTITY_TYPE_LAVA][i]);
		if( lavaEntity->m_currentPositionIndex == IntVec2( tileX, tileY ) && !lavaEntity->IsFilled() )
		{
			return false;
		}
	}

	for( int i = 0; i < (int)m_entityListsByType[ENTITY_TYPE_PET].size(); ++i )
	{
		Entity* petEntity = m_entityListsByType[ENTITY_TYPE_PET][i];
		if( petEntity->m_currentPositionIndex == IntVec2( tileX, tileY ) )
		{
			return false;
		}
	}

	environmentEntity->Move( tileX, tileY );
	g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/stone_move_01.mp3" ), false, g_theGame->m_volume );
	return true;
}

bool Map::MoveFireballIfPossible( Entity* fireballEntity, const IntVec2& direction )
{
	IntVec2 targetIndex = fireballEntity->m_currentPositionIndex + direction;
	if( IsOutOfBounds( targetIndex ) )
	{
		fireballEntity->m_isReadyToBeDeleted = true;
		return false;
	}
	else
	{
		for( int i = 0; i < (int)m_entityListsByType[ENTITY_TYPE_BREAKABLE_OBJECT].size(); ++i )
		{
			Entity* breakableEntity = m_entityListsByType[ENTITY_TYPE_BREAKABLE_OBJECT][i];
			if( breakableEntity->m_currentPositionIndex == targetIndex )
			{
				// breaking object
				fireballEntity->m_isReadyToBeDeleted = true;
				breakableEntity->m_isReadyToBeDeleted = true;
				g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/stone_break_01.mp3" ), false, g_theGame->m_volume );

				// check is there have lava behind
				for( int lavaIndex = 0; lavaIndex < (int)m_entityListsByType[ENTITY_TYPE_LAVA].size(); ++lavaIndex )
				{
					Lava* lavaEntity = dynamic_cast<Lava*>(m_entityListsByType[ENTITY_TYPE_LAVA][lavaIndex]);
					if( lavaEntity->m_currentPositionIndex == targetIndex + direction )
					{
						lavaEntity->FillLava();
					}
				}
				return false;
			}
		}

		for( int i = 0; i < (int)m_entityListsByType[ENTITY_TYPE_UNBREAKABLE_OBJECT].size(); ++i )
		{
			Entity* unbreakableEntity = m_entityListsByType[ENTITY_TYPE_UNBREAKABLE_OBJECT][i];

			if( unbreakableEntity->m_currentPositionIndex == targetIndex )
			{
				fireballEntity->m_isReadyToBeDeleted = true;
				return false;
			}
		}

		fireballEntity->Move( targetIndex.x, targetIndex.y );
	}

	return true;
}

Vec2 Map::GetTileCenterPosition( int tileX, int tileY ) const
{
	AABB2 bound = m_tiles[GetTileIndexForTileCoords(tileX, tileY)].GetBounds();
	return bound.GetCenter();
}

void Map::Update( float deltaSeconds )
{
	if( m_cutscenePlayer.m_isPlayingCutscene )
	{
		UpdateCutscene( deltaSeconds );
	}
	else
	{
		Entity* player = m_entityListsByType[ENTITY_TYPE_PLAYER][0];
		m_playerDust->m_position = Vec3( player->m_position, 0.f ) + Vec3( 0.f, -0.3f, 0.f );
		UpdateTiles( deltaSeconds );
		UpdateEntities( deltaSeconds );
		m_particleSystem->Update( deltaSeconds );
	}
}

void Map::UpdateCutscene( float deltaSeconds )
{
	if( m_cutscenePlayer.m_currentCutscene == nullptr )
	{
		m_cutscenePlayer.m_isPlayingCutscene = false;
		return;
	}
	m_cutscenePlayer.Updated( deltaSeconds );

	if ( g_theInput->WasKeyJustPressed( VK_SPACE ) && !m_game->m_world->m_isExitAlertShowing )
	{
		std::string line = m_cutscenePlayer.m_currentCutscene->m_lines[m_cutscenePlayer.m_currentLineIndex]->m_line;
		if( line.size() == m_cutscenePlayer.m_lineStrIndex - 1 )
		{
			m_cutscenePlayer.NextCutsceneLine();
		}
		else
		{
			m_cutscenePlayer.m_lineStrIndex = (int)line.size() - 1;
			m_cutscenePlayer.m_line = line;
		}
	}
}

void Map::UpdateTiles( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}

void Map::UpdateEntities( float deltaSeconds )
{
	for( int entityTypeIndex = 0; entityTypeIndex < NUM_ENTITY_TYPES; entityTypeIndex++ )
	{
		EntityList entityLists = m_entityListsByType[entityTypeIndex];
		for( int entityIndex = 0; entityIndex < (int) entityLists.size(); entityIndex++ )
		{
			if ( entityLists[entityIndex] )
			{
				entityLists[entityIndex]->Update( deltaSeconds );
			}
		}
	}
}

void CutscenePlayer::Updated( float deltaSeconds )
{
	m_age += deltaSeconds;
	if ( m_age >= 0.02f )
	{
		m_age = 0.f;
		std::string line = m_currentCutscene->m_lines[m_currentLineIndex]->m_line;
		if ( line.size() != m_lineStrIndex - 1 )
		{
			m_line = line.substr( 0, m_lineStrIndex );
			for ( int i = 0; i < (int) line.size() - m_lineStrIndex; ++i )
			{
				m_line += " ";
			}
			m_lineStrIndex++;
		}
	}
}

void CutscenePlayer::NextCutsceneLine()
{
	int linesLength = (int)m_currentCutscene->m_lines.size();
	++m_currentLineIndex;
	g_theAudio->PlaySound( g_theAudio->CreateOrGetSound( "Data/Audio/button_dialogue_highlight_01.mp3" ), false, g_theGame->m_volume );
	m_line = "";
	m_lineStrIndex = 0;
	if( m_currentLineIndex == linesLength )
	{
		m_currentLineIndex = 0;
		m_currentCutscene = nullptr;
		m_isPlayingCutscene = false;
		if ( "" != m_afterPlayingEventName )
		{
			g_theEventSystem->FireEvent( m_afterPlayingEventName );
		}
	}
}
