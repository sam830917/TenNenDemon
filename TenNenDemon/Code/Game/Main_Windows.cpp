#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "App.hpp"
#include "Engine/Platform/Window.hpp"

App* g_theApp = nullptr;				// Created and owned by Main_Windows.cpp
Window* g_theWindow = nullptr;

//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move these constants to Game/GameCommon.hpp or elsewhere
// 
#define UNUSED(x) (void)(x);


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move each of these items to its proper place, once that place is established
// 
//HWND g_hWnd = nullptr;								// ...becomes WindowContext::m_windowHandle
//HDC g_displayDeviceContext = nullptr;				// ...becomes WindowContext::m_displayContext
//HGLRC g_openGLRenderingContext = nullptr;			// ...becomes RenderContext::m_apiRenderingContext
const char* APP_NAME = "Protogame";	// ...becomes ??? (Change this per project!)
constexpr float CLIENT_ASPECT = 2.0f;

//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( _In_ HINSTANCE applicationInstanceHandle, _In_opt_ HINSTANCE , _In_ LPSTR commandLineString, _In_ int )
{
	UNUSED( applicationInstanceHandle );
	UNUSED( commandLineString );

	g_gameConfigBlackboard.PopulateFromXmlFile( "Data/GameConfig.xml" );
	g_theWindow = new Window();

	g_theWindow->Open( g_gameConfigBlackboard.GetValue( "appName", APP_NAME ), g_gameConfigBlackboard.GetValue( "windowAspect", CLIENT_ASPECT ), 0.9f );

	//TheApp_Startup( applicationInstanceHandle, commandLineString );
	g_theApp = new App();
	g_theApp->Startup();

	// Program main loop; keep running frames until it's time to quit
	while( !g_theApp->IsQuitting() )
	{
		g_theWindow->BeginFrame();
		g_theApp->RunFrame();
	}

	g_theApp->Shutdown();
	delete g_theApp;
	g_theApp = nullptr;

	g_theWindow->Close();
	delete g_theWindow;
	g_theWindow = nullptr;

	return 0;
}
