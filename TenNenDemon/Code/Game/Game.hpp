#pragma once
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/AABB2.hpp"
#include "GameCommon.hpp"

class World;
class GameObject;
class GPUMesh;
class Material;
class TextureCube;
class World;
class Map;
class SpriteAnimDefinition;
class ParticleSystem;

class Game
{
public: 
	Game();
	~Game();

	void Startup();

	void Update( float deltaSeconds );
	void UpdatePlayingLevel( float deltaSeconds );
	void UpdateMainMenu( float deltaSeconds );
	void UpdateLevelSelect( float deltaSeconds );
	void UpdateSettings( float deltaSeconds );
	void UpdateControls( float deltaSeconds );

	void Shutdown();

	void Render() const;
	void RenderMainMenu() const;
	void RenderPlayingLevel() const;
	void RenderLevelSelect() const;
	void RenderSettings() const;
	void RenderControls() const;
	void RenderDebug() const;

	void UpdateBasicInput();
	void ChangeGameState( eGameState gameState );
	Map* GetCurrentMap() const;
	Camera* GetWorldCamera() const { return m_worldCamera; }
	void LoadAllAudio( const char* folderPath ) const;

public:
	RandomNumberGenerator m_rng;
	bool m_isPause = false;
	bool m_isCheatMode = false;
	World* m_world = nullptr;
	Camera* m_worldCamera = nullptr;
	float m_volume = 1.f;

private:
	Camera* m_devConsoleCamera = nullptr;

	Clock* m_clock = nullptr;
	Vec3 m_focalPoint = CAMERA_START_POSITION;
	Vec3 m_cameraPosition;

	std::vector<GameObject*> m_gameObjects;

	int m_uiButtonIndex = 0;
	SpriteAnimDefinition* m_bgAnim = nullptr;
	SoundPlaybackID m_currentAudio;
	ParticleSystem* m_particleSystem = nullptr;
};