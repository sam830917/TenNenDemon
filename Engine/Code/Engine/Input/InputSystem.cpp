#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Platform/Window.hpp"

extern Window* g_theWindow;

InputSystem::InputSystem()
{
}

InputSystem::~InputSystem()
{
}

void InputSystem::StartUp()
{
}

void InputSystem::BeginFrame()
{
	// Update each of the system¡¦s 4 XboxControllers
	UpdateMouse();
	m_controllers->Update();
	if ( m_mousePositionMode == MOUSE_MODE_RELATIVE )
	{
		UpdateRelativeMode();
	}
}

void InputSystem::EndFrame()
{
	for( int keyCode = 0; keyCode < NUM_KEYCODES; keyCode++ )
	{
		KeyButtonState& keystate = m_keyStates[keyCode];
		m_keyStates[ keyCode ].UpdateStatus( keystate.IsPressed() );
	}
	m_mouseLeftButton.UpdateStatus( m_mouseLeftButton.IsPressed() );
	m_mouseMiddleButton.UpdateStatus( m_mouseMiddleButton.IsPressed() );
	m_mouseRightButton.UpdateStatus( m_mouseRightButton.IsPressed() );
	m_mouseWheelScrollAmount = 0.f;
	char c;
	while( PopCharacter( &c ) ) {}
}

void InputSystem::Shutdown()
{
}

bool InputSystem::HandleKeyPressed( unsigned char keyCode )
{
	m_keyStates[ keyCode ].UpdateStatus( true );
	return true;
}

bool InputSystem::HandleKeyReleased( unsigned char keyCode )
{
	m_keyStates[keyCode].UpdateStatus( false );
	return true;
}

bool InputSystem::IsKeyPressed( unsigned char keyCode ) const
{
	return m_keyStates[keyCode].IsPressed();
}

bool InputSystem::WasKeyJustPressed( unsigned char keyCode ) const
{
	return m_keyStates[keyCode].WasJustPressed();
}

bool InputSystem::WasKeyJustReleased( unsigned char keyCode ) const
{
	return m_keyStates[keyCode].WasJustReleased();
}

bool InputSystem::IsMouseButtonPressed( eMouseButton btn ) const
{
	switch( btn )
	{
	case LEFT_BUTTON:		return m_mouseLeftButton.IsPressed();
	case MIDDLE_BUTTON:		return m_mouseMiddleButton.IsPressed();
	case RIGHT_BUTTON:		return m_mouseRightButton.IsPressed();
	default: return false;
	}
}

bool InputSystem::WasMouseButtonJustPressed( eMouseButton btn ) const
{
	switch( btn )
	{
	case LEFT_BUTTON:		return m_mouseLeftButton.WasJustPressed();
	case MIDDLE_BUTTON:		return m_mouseMiddleButton.WasJustPressed();
	case RIGHT_BUTTON:		return m_mouseRightButton.WasJustPressed();
	default: return false;
	}
}

bool InputSystem::WasMouseButtonJustReleased( eMouseButton btn ) const
{
	switch( btn )
	{
	case LEFT_BUTTON:		return m_mouseLeftButton.WasJustReleased();
	case MIDDLE_BUTTON:		return m_mouseMiddleButton.WasJustReleased();
	case RIGHT_BUTTON:		return m_mouseRightButton.WasJustReleased();
	default: return false;
	}
}

const XboxController& InputSystem::GetXboxController( int controllerID )
{
	return m_controllers[ controllerID ];
}

const KeyButtonState& InputSystem::GetKeyButtonState( unsigned char keyCode )
{
	return m_keyStates[ keyCode ];
}

const Vec2 InputSystem::GetMouseNormalizedClientPos()
{
	return m_mouseNormalizedClientPos;
}

bool InputSystem::PopCharacter( char* out )
{
	if ( m_characters.size() != 0 )
	{
		*out = m_characters.front();
		m_characters.pop();
		return true;
	}
	return false;
}

void InputSystem::InputCharacter( unsigned char c )
{
	m_characters.push(c);
}

void InputSystem::UpdateMouse()
{
	HWND hwnd = (HWND) g_theWindow->m_hwnd;

	POINT mousePos;
	GetCursorPos( &mousePos );
	ScreenToClient( hwnd, &mousePos );
	Vec2 mouseClientPos( (float) mousePos.x, (float) mousePos.y );
	
	RECT clientRect;
	GetClientRect( hwnd, &clientRect );
	AABB2 clientBounds( (float) clientRect.left, (float) clientRect.top, (float) clientRect.right, (float) clientRect.bottom );
	m_mouseNormalizedClientPos = clientBounds.GetUVForPoint( mouseClientPos );
	m_mouseNormalizedClientPos.y = 1.f - m_mouseNormalizedClientPos.y;
}

void InputSystem::UpdateRelativeMode()
{
	Vec2 positionThisFrame = Vec2( 1.f - m_mouseNormalizedClientPos.x, m_mouseNormalizedClientPos.y );
	m_relativeMovement = positionThisFrame - m_positionLastFrame;

	// move back to center
	HWND hwnd = (HWND)g_theWindow->m_hwnd;
	RECT clientRect;
	GetClientRect( hwnd, &clientRect );
	POINT clientScreen = { 0,0 };
	ScreenToClient( hwnd, &clientScreen );
	SetCursorPos( (int)(clientRect.right * 0.5f) - clientScreen.x, (int)(clientRect.bottom * 0.5f) - clientScreen.y );
	UpdateMouse();

	// recenter
	m_positionLastFrame = Vec2( 1.f - m_mouseNormalizedClientPos.x, m_mouseNormalizedClientPos.y );
}

void InputSystem::UpdateMouseButtonState( bool leftButtonDown, bool middleButtonDown, bool rightButtonDown )
{
	m_mouseLeftButton.UpdateStatus( leftButtonDown );
	m_mouseMiddleButton.UpdateStatus( middleButtonDown );
	m_mouseRightButton.UpdateStatus( rightButtonDown );
}

void InputSystem::AddMouseWheelScrollAmount( float scrollAmount )
{
	m_mouseWheelScrollAmount = scrollAmount;
}

void InputSystem::HideSystemCursor() const
{
	while (ShowCursor(FALSE) > 0) {}
}

void InputSystem::ShowSystemCursor() const
{
	while (ShowCursor(TRUE) < 0) {}
}

void InputSystem::ClipSystemCursor() const
{
	HWND hwnd = (HWND)g_theWindow->m_hwnd;

	RECT clientRect;
	GetClientRect( hwnd, &clientRect );
	POINT relativePos = { 0, 0 };
	ScreenToClient( hwnd, &relativePos );
	SetRect( &clientRect, clientRect.left - relativePos.x, clientRect.top - relativePos.y, clientRect.right - relativePos.x, clientRect.bottom - relativePos.y );
	ClipCursor( &clientRect );
}

void InputSystem::UnClipSystemCursor() const
{
	ClipCursor( nullptr );
}

void InputSystem::SetCursorMode( eMousePositionMode mode )
{
	m_mousePositionMode = mode;
	switch( mode )
	{
	case MOUSE_MODE_ABSOLUTE:
		ShowSystemCursor();
		UnClipSystemCursor();
		break;
	case MOUSE_MODE_RELATIVE:
		HideSystemCursor();
		ClipSystemCursor();
		break;
	default:	ERROR_AND_DIE( "UNKNOWN MOUSE MODE!" );
	}
}

void InputSystem::SetDefaultMouseMode( eMousePositionMode mode )
{
	m_defaultMouseMode = mode;
}

void InputSystem::ApplyCurrentState()
{
	if ( (int) m_modeStack.size() > 0 )
	{
		MouseModeSettings settings = m_modeStack.top();

		m_mousePositionMode = settings.mode;
		settings.isMouseClipped ? ClipSystemCursor() : UnClipSystemCursor();
		settings.isMouseVisible ? ShowSystemCursor() : HideSystemCursor();
	}
	else
	{
		SetCursorMode( m_defaultMouseMode );
	}
}

void InputSystem::PushMouseOptions( eMousePositionMode mode, bool visible, bool clipped )
{
	MouseModeSettings mouseModeSettings;
	mouseModeSettings.mode = mode;
	mouseModeSettings.isMouseClipped = clipped;
	mouseModeSettings.isMouseVisible = visible;

	m_modeStack.push( mouseModeSettings );
	ApplyCurrentState();
}

void InputSystem::PopMouseOptions()
{
	m_modeStack.pop();
	ApplyCurrentState();
}
