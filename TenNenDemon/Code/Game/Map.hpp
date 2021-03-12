#pragma once
#include <vector>
#include "Game/Entity/Entity.hpp"
#include "Tile.hpp"

class World;
class Player;
class Clock;
class Timer;
class MapDefinition;
class EnvironmentEntityDefinition;
class CutsceneDefinition;
class SpriteAnimDefinition;
class ParticleSystem;
class Emitter;

struct CutscenePlayer
{
public:
	const CutsceneDefinition*	m_currentCutscene = nullptr;
	bool						m_isPlayingCutscene = false;
	int							m_currentLineIndex = 0;
	std::string					m_afterPlayingEventName = "";
	std::string					m_line = "";
	SpriteAnimDefinition*		m_booperAnim = nullptr;
	float						m_age = 0.f;

	int							m_lineStrIndex = 0;

public:
	void Updated( float deltaSeconds );

	void NextCutsceneLine();
	bool IsPlayingCutscene() const { return m_isPlayingCutscene; }
};

class Map
{
	friend class World;
	friend class Game;

public:
	Entity* SpawnNewEntity( const std::string& name, const Vec2& pos, eEntityType entityType );
	void SpawnNewEnvironmentEntity( const Vec2& pos, EnvironmentEntityDefinition* def, eEntityType entityType );
	void UseFireballAbility( const IntVec2& direction );
	void PlayCutscene( const CutsceneDefinition* cutsceneDef, const std::string& afterPlayingEventName = "" );

	bool IsEntityAlive( const Entity* entity ) const;
	bool IsOutOfBounds( int tileX, int tileY );
	bool IsOutOfBounds( const IntVec2& tileCoords ) const;
	int GetTileIndexForTileCoords( const IntVec2& tileCoords ) const;
	int GetTileIndexForTileCoords( int tileX, int tileY ) const;
	IntVec2 GetTileCoordsForWorldPos( const Vec2& worldPos ) const;

	bool MovePlayerIfPossible( int directionX, int directionY );
	bool MovePlayerIfPossible( const IntVec2& direction ) { return MovePlayerIfPossible( direction.x, direction.y ); }
	bool MoveEnvironmentEntityIfPossible( Entity* environmentEntity, int tileX, int tileY );
	bool MoveFireballIfPossible( Entity* fireballEntity, const IntVec2& direction );

	Vec2 GetTileCenterPosition( int tileX, int tileY ) const;
	Vec2 GetTileCenterPosition( const IntVec2& tileCoords ) const { return GetTileCenterPosition( tileCoords.x, tileCoords.y ); }
	int GetAbilityLimitNumber() const { return m_abilityLimitNumber; }

private:
	Map( Game* game, const std::string& name );
	~Map();

	void Update( float deltaSeconds );
	void UpdateCutscene( float deltaSeconds );
	void UpdateTiles( float deltaSeconds );
	void UpdateEntities( float deltaSeconds );

	void Render() const;
	void RenderCutscene() const;
	void RenderBackground() const;
	void RenderUI() const;
	void RenderEntities() const;
	void DebugModeRender() const;

	void ClearDeadEntities();

	void AddEntityToMap( Entity* entity );
	void PopulateTiles();
	void CheckIsWinning();

public:
	Game*		m_game;
	std::string	m_name;
	std::string	m_levelName;
	Clock* m_mapClock = nullptr;
	MapDefinition* m_mapDef = nullptr;
	std::vector< Tile > m_tiles;

	EntityList	m_entityListsByType[ NUM_ENTITY_TYPES ];
	IntVec2		m_tileDimensions;
	int			m_abilityLimitNumber = 0;

	CutscenePlayer m_cutscenePlayer;
	Timer* m_successTimer = nullptr;
	SpriteAnimDefinition* m_successAnim = nullptr;
	ParticleSystem* m_particleSystem = nullptr;
	Emitter* m_playerDust = nullptr;
};