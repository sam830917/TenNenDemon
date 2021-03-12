#include "App.hpp"
#include "GameCommon.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/DebugRender.hpp"

RenderContext* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;
DevConsole* g_theConsole = nullptr;
EventSystem* g_theEventSystem = nullptr;
AudioSystem* g_theAudio = nullptr;

extern App* g_theApp;

eGameState g_gameState = MAIN_MENU;
bool g_isDebugMode = false;
bool g_isDebugCamera = false;

App::App()
{
}

App::~App()
{
}

void App::Startup()
{
	Clock::SystemStartup();

	g_theRenderer = new RenderContext();
	g_theInput = new InputSystem();
	g_theConsole = new DevConsole();
	g_theEventSystem = new EventSystem();
	g_theAudio = new AudioSystem();
	g_theGame = new Game();

	DebugRenderSystemStartup();
	g_theRenderer->Startup( g_theWindow );
	g_theInput->StartUp();
	g_theConsole->Startup();
	g_theAudio->Startup();
	g_theGame->Startup();

	g_theWindow->SetInputSystem( g_theInput );
	g_theWindow->SetEventSystem( g_theEventSystem );
}

void App::Shutdown()
{
	g_theRenderer->Shutdown();
	g_theGame->Shutdown();
	g_theAudio->Shutdown();
	DebugRenderSystemShutdown();
}

void App::RunFrame()
{
	static double timeLastFrameStarted = GetCurrentTimeSeconds();
	double timeThisFrameStarted = GetCurrentTimeSeconds();
	double deltaSeconds = timeThisFrameStarted - timeLastFrameStarted;
	timeLastFrameStarted = timeThisFrameStarted;
	// Clamped to a maximum deltaSeconds of 0.1f
	deltaSeconds > 0.1f?deltaSeconds = 0.1f:true;

	Clock::GetMaster()->Update( deltaSeconds );
	BeginFrame();
	Update( ( float ) deltaSeconds );
	Render();
	EndFrame();
}

bool App::IsQuitting() const
{
	return m_isQuitting;
}

bool App::HandleKeyPressed( unsigned char keyCode )
{
	g_theInput->HandleKeyPressed( keyCode );
	return false;
}

bool App::HandleKeyReleased( unsigned char keyCode )
{
	g_theInput->HandleKeyReleased( keyCode );
	return false;
}

bool App::HandleQuitRequested()
{
	m_isQuitting = true;
	return false;
}

void App::JoystickButtonControl()
{
	const XboxController& controller = g_theInput->GetXboxController( 0 );
	if( !controller.IsConnected() )
		return;
}

void App::BeginFrame()
{
	g_theRenderer->BeginFrame();
	g_theInput->BeginFrame();
	g_theConsole->BeginFrame();
	g_theAudio->BeginFrame();
	DebugRenderBeginFrame();
}

void App::EndFrame()
{
	g_theInput->EndFrame();
	g_theRenderer->EndFrame();
	g_theConsole->EndFrame();
	g_theAudio->EndFrame();
	DebugRenderEndFrame();
}

void App::Update( float deltaSeconds )
{
	g_theRenderer->UpdateFrameTime( deltaSeconds );
	JoystickButtonControl();
	g_theConsole->Update( deltaSeconds );
	g_theGame->Update( deltaSeconds );
}

void App::Render() const
{
	g_theGame->Render();
	g_theGame->RenderDebug();
}

COMMAND( quit, "Quit the game.", "" )
{
	UNUSED( args );
	g_theApp->HandleQuitRequested();
}