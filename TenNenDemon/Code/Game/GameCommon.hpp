#pragma once
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"

#define VK_LEFT           0x25
#define VK_UP             0x26
#define VK_RIGHT          0x27
#define VK_DOWN           0x28
#define VK_SPACE          0x20
#define VK_F1             0x70
#define VK_F2             0x71
#define VK_F3             0x72
#define VK_F4             0x73
#define VK_F8             0x77
#define VK_ESCAPE         0x1B
#define VK_OEM_3          0xC0   // '`~' for US
#define VK_RETURN         0x0D	 // enter
#define VK_SHIFT          0x10

#define UNUSED(x) (void)(x);

enum eGameState
{
	MAIN_MENU,
	LEVEL_SELECT,
	SETTINGS,
	CONTROLS,
	PLAY_MODE
};

class Window;
class Game;
class ParticleSystem;

extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;
extern EventSystem* g_theEventSystem;
extern AudioSystem* g_theAudio;
extern Game* g_theGame;
extern Window* g_theWindow;

extern bool g_isDebugMode;
extern bool g_isDebugCamera;
extern eGameState g_gameState;

constexpr float SCREEN_ASPECT = 2.f;
constexpr float UI_SIZE_X = 1600.f;
constexpr float UI_SIZE_Y = 800.f;
constexpr int MAP_SIZE_X = 20;
constexpr int MAP_SIZE_Y = 30;
constexpr float WORLD_SIZE_X = 20.f;
constexpr float WORLD_SIZE_Y = 10.f;

constexpr int TOO_FAR_DISTANCE = 25;
constexpr int EXIST_BOX_NUMBER = 20;
constexpr float SUB_LIVING_TIME_SPEED = 0.1f;

constexpr float PLAYER_SIZE_X = 1.f;
constexpr float PLAYER_SIZE_Y = 0.5f;
constexpr float TANK_SPEED = 2.f;
constexpr float TANK_ROTATION_SPEED = 180.f;
constexpr float TANK_PHYSICS_RADIUS = 0.3f;
constexpr float TANK_COSMETIC_RADIUS = 0.5f;

constexpr float DEFAULT_PHYSICS_RADIUS = 0.3f;
constexpr float DEFAULT_DRAW_RADIUS = 0.5f;

constexpr float MAX_CAMERA_SHAKE_DIST = 3.f;
constexpr float SCREEN_SHAKE_ABLATION_PER_SECOND = 1.f;
const Vec3 CAMERA_START_POSITION = Vec3::ZERO;

void DrawLine( const Vec2& startVec2, const Vec2& endVec2, const Rgba8& color, float thickness );
void DrawRing( const Vec2& centerVec2, float radius, const Rgba8& color, float thickness );
void AppendVertsForAABB2D( std::vector<Vertex_PCU>& vertVector, const AABB2& localBounds, const Rgba8& color );
void AddVertsForAABB2( std::vector<Vertex_PCU>& verts, const AABB2& localBounds, const Rgba8& color, const Vec2& uvMins, const Vec2& uvMaxs );