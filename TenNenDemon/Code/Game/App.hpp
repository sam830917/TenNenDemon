#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game.hpp"


class App
{
public:
	App();
	~App();

	void Startup();
	void Shutdown();
	void RunFrame();

	void JoystickButtonControl();
	bool IsQuitting() const;
	bool HandleKeyPressed( unsigned char keyCode );
	bool HandleKeyReleased( unsigned char keyCode );
	bool HandleQuitRequested();

private:
	void BeginFrame();
	void EndFrame();
	void Update( float deltaSeconds );
	void Render() const;

private:
	bool m_isQuitting = false;

};