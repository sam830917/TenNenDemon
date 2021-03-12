#pragma once
#include "Engine/Input/XboxController.hpp"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <queue>
#include <stack>

extern HWND g_hWnd;

constexpr int MAX_XBOX_CONTROLLERS = 4;
constexpr int NUM_KEYCODES = 250;

#define VK_LEFT						0x25
#define VK_UP						0x26
#define VK_RIGHT					0x27
#define VK_DOWN						0x28
#define VK_SPACE					0x20
#define VK_F1						0x70
#define VK_F2						0x71
#define VK_F3						0x72
#define VK_F4						0x73
#define VK_F5						0x74
#define VK_F6						0x75
#define VK_F7						0x76
#define VK_F8						0x77
#define VK_F9						0x78
#define VK_ESCAPE					0x1B
#define VK_OEM_3					0xC0  // '`~' for US
#define VK_RETURN					0x0D	// enter
#define VK_BACK						0x08  // backspace
#define VK_DELETE					0x2E	// DEL key
#define VK_CONTROL					0x11
#define VK_MENU						0x12	// ALT
#define KEY_CUT						0x18
#define KEY_COPY					0x03
#define KEY_PASTE					0x16
#define KEY_CTRL_A					0x01
#define VK_HOME						0x24
#define VK_END						0x23
#define VK_CONTROL					0x11
#define VK_OEM_COMMA				0xBC	// <
#define VK_OEM_PERIOD				0xBE	// >
#define UP_SQUARE_BRACKETS			0xDB	// [
#define DOWN_SQUARE_BRACKETS		0xDD	// ]
#define VK_OEM_1					0xBA	// : ;
#define VK_OEM_7					0xDE	// ' "
#define VK_OEM_PLUS					0xBB	// '+'
#define VK_OEM_MINUS				0xBD	// '-'

enum eMouseButton
{
	LEFT_BUTTON,
	MIDDLE_BUTTON,
	RIGHT_BUTTON,
};
enum eMousePositionMode
{
	MOUSE_MODE_ABSOLUTE,	// Moves around freely 
	MOUSE_MODE_RELATIVE,
};
struct MouseModeSettings
{
	eMousePositionMode mode = MOUSE_MODE_ABSOLUTE;
	bool isMouseVisible = true;
	bool isMouseClipped = false;
};

class InputSystem
{
public:
	InputSystem();
	~InputSystem();

	void StartUp();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	bool HandleKeyPressed( unsigned char keyCode );
	bool HandleKeyReleased( unsigned char keyCode );
	void UpdateMouseButtonState( bool leftButtonDown, bool middleButtonDown, bool rightButtonDown );
	void AddMouseWheelScrollAmount( float scrollAmount );
	void ShowSystemCursor() const;
	void HideSystemCursor() const;
	void ClipSystemCursor() const;
	void UnClipSystemCursor() const;

	void SetCursorMode( eMousePositionMode mode );
	void SetDefaultMouseMode( eMousePositionMode mode );
	eMousePositionMode GetDefaultMouseMode() { return m_defaultMouseMode; }
	void ApplyCurrentState();
	void PushMouseOptions( eMousePositionMode mode, bool visible, bool clipped );
	void PopMouseOptions();

	// Key event
	bool IsKeyPressed( unsigned char keyCode ) const;
	bool WasKeyJustPressed( unsigned char keyCode ) const;
	bool WasKeyJustReleased( unsigned char keyCode ) const;

	// Mouse event
	bool IsMouseButtonPressed( eMouseButton btn ) const;
	bool WasMouseButtonJustPressed( eMouseButton btn ) const;
	bool WasMouseButtonJustReleased( eMouseButton btn ) const;
	float GetMouseWheelScrollAmount() const { return m_mouseWheelScrollAmount; }
	const Vec2 GetMouseNormalizedClientPos();
	const Vec2 GetMouseRelativeMovement() { return m_relativeMovement; }
	
	const XboxController& GetXboxController( int controllerID );
	const KeyButtonState& GetKeyButtonState( unsigned char keyCode );

	// Typing
	bool PopCharacter( char* out );
	void InputCharacter( unsigned char c );

private:
	void UpdateMouse();
	void UpdateRelativeMode();

private:
	// Key Input
	std::queue<char> m_characters;
	KeyButtonState m_keyStates[ NUM_KEYCODES ];
	XboxController m_controllers[ MAX_XBOX_CONTROLLERS ] = 
	{
		XboxController(0),
		XboxController(1),
		XboxController(2),
		XboxController(3)
	};

	// Mouse
	KeyButtonState m_mouseLeftButton;
	KeyButtonState m_mouseMiddleButton;
	KeyButtonState m_mouseRightButton;
	float m_mouseWheelScrollAmount = 0.f;
	Vec2 m_mouseNormalizedClientPos = Vec2::ZERO;
	Vec2 m_positionLastFrame = Vec2::ZERO;
	Vec2 m_relativeMovement = Vec2::ZERO;

	eMousePositionMode m_mousePositionMode = MOUSE_MODE_ABSOLUTE;
	eMousePositionMode m_defaultMouseMode = MOUSE_MODE_ABSOLUTE;

	std::stack<MouseModeSettings> m_modeStack;
};