#include "Engine/Input/XboxController.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // must #include Windows.h before #including Xinput.h
#include <Xinput.h> // include the Xinput API header file (interface)
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; use version 9_1_0 explicitly for broadest compatibility


XboxController::XboxController( int controllerId )
	:m_controllerID(controllerId)
{
}

XboxController::~XboxController()
{
}

const KeyButtonState& XboxController::GetButtonState( XboxButtonID buttonID ) const
{
	return m_buttonStates[ buttonID ];
}

void XboxController::Update()
{
	// Read raw controller state directly from XInput API
	XINPUT_STATE xboxControllerState;    // Current state
	memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );  // The function works similar as ZeroMemory, clear state every update()
	DWORD errorStatus = XInputGetState( m_controllerID, &xboxControllerState );  // Place all the button pressed into state

	if( errorStatus == ERROR_SUCCESS )
	{
		m_isConnected = true;
		XINPUT_GAMEPAD& gamepad = xboxControllerState.Gamepad;
		UpdateJoystick( m_leftJoystick, gamepad.sThumbLX, gamepad.sThumbLY );
		UpdateJoystick( m_rightJoystick, gamepad.sThumbRX, gamepad.sThumbRY );
		UpdateTrigger( m_leftTriggerValue, gamepad.bLeftTrigger );
		UpdateTrigger( m_rightTriggerValue, gamepad.bRightTrigger );
		UpdateButton( XBOX_BUTTON_ID_A, gamepad.wButtons, XINPUT_GAMEPAD_A );
		UpdateButton( XBOX_BUTTON_ID_B, gamepad.wButtons, XINPUT_GAMEPAD_B );
		UpdateButton( XBOX_BUTTON_ID_X, gamepad.wButtons, XINPUT_GAMEPAD_X );
		UpdateButton( XBOX_BUTTON_ID_Y, gamepad.wButtons, XINPUT_GAMEPAD_Y );
		UpdateButton( XBOX_BUTTON_ID_BACK, gamepad.wButtons, XINPUT_GAMEPAD_BACK );
		UpdateButton( XBOX_BUTTON_ID_START, gamepad.wButtons, XINPUT_GAMEPAD_START );
		UpdateButton( XBOX_BUTTON_ID_LSHOULDER, gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER );
		UpdateButton( XBOX_BUTTON_ID_RSHOULDER, gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER );
		UpdateButton( XBOX_BUTTON_ID_LTHUMB, gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB );
		UpdateButton( XBOX_BUTTON_ID_RTHUMB, gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB );
		UpdateButton( XBOX_BUTTON_ID_DPAD_RIGHT, gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT );
		UpdateButton( XBOX_BUTTON_ID_DPAD_UP, gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP );
		UpdateButton( XBOX_BUTTON_ID_DPAD_LEFT, gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT );
		UpdateButton( XBOX_BUTTON_ID_DPAD_DOWN, gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN );
	}
	else if( errorStatus == ERROR_DEVICE_NOT_CONNECTED )
	{
		m_isConnected = false;
		Reset();
	}
}

void XboxController::Reset()
{
	m_buttonStates[ XBOX_BUTTON_ID_A ].Reset();
	m_buttonStates[ XBOX_BUTTON_ID_B ].Reset();
	m_buttonStates[ XBOX_BUTTON_ID_X ].Reset();
	m_buttonStates[ XBOX_BUTTON_ID_Y ].Reset();
	m_buttonStates[ XBOX_BUTTON_ID_BACK ].Reset();
	m_buttonStates[ XBOX_BUTTON_ID_START ].Reset();
	m_buttonStates[ XBOX_BUTTON_ID_LSHOULDER ].Reset();
	m_buttonStates[ XBOX_BUTTON_ID_RSHOULDER ].Reset();
	m_buttonStates[ XBOX_BUTTON_ID_LTHUMB ].Reset();
	m_buttonStates[ XBOX_BUTTON_ID_RTHUMB ].Reset();
	m_buttonStates[ XBOX_BUTTON_ID_DPAD_RIGHT ].Reset();
	m_buttonStates[ XBOX_BUTTON_ID_DPAD_UP ].Reset();
	m_buttonStates[ XBOX_BUTTON_ID_DPAD_LEFT ].Reset();
	m_buttonStates[ XBOX_BUTTON_ID_DPAD_DOWN ].Reset();
}

void XboxController::UpdateTrigger( float& triggerValuem, unsigned char rawValue )
{
	triggerValuem = static_cast<float>(rawValue) / 255.f;
}

void XboxController::UpdateJoystick( AnalogJoystick& joystick, short rawX, short rawY )
{
	joystick.m_rawPosition = Vec2( rawX, rawY );
	float rawNormalizedX = rawX > 0 ? rawX / 32767.f : rawX / 32768.f;
	float rawNormalizedY = rawY > 0 ? rawY / 32767.f : rawY / 32768.f;
	joystick.UpdatePosition( rawNormalizedX, rawNormalizedY );
}

void XboxController::UpdateButton( XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag )
{
	if ( (buttonFlags & buttonFlag) != 0 )
	{
		m_buttonStates[ buttonID ].UpdateStatus( true );
	}
	else 
	{
		m_buttonStates[buttonID].UpdateStatus( false );
	}
}
