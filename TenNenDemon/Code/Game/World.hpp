#pragma once
#include "Map.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>
#include <string>

class Game;

class World
{
	friend class Map;
	friend class Game;

public:
	World( Game* game );
	~World();

	void Update( float deltaSeconds );
	void Render() const;
	void DebugModeRender() const;

	void EndFrame();

	void CreateNewMap( const std::string& mapName );
	void WinCurrentLevel();
	void RestartCurrentMap( bool needPlayCutscene = true );

	void EnterMap( const std::string& mapName );
	void NextMap();

	bool IsPlayerControllable() const;
	bool IsWinInCurrentLevel() const { return m_isJustWinCurrentLevel; }
	Map* GetCurrentMap() { return m_currentMap; }

private:
	Map* m_currentMap = nullptr;
	Game* m_game = nullptr;
	int m_currentMapIndex = 0;
	bool m_isJustWinCurrentLevel = false;
	std::string m_currentMapName = "";
	bool m_isExitAlertShowing = false;

};