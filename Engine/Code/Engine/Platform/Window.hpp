#pragma once
#include <string>
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EventSystem.hpp"

class Window
{
public:
	Window();
	~Window();

	bool Open( std::string const& title, float clientAspect, float maxClientFractionOfDesktop );
	void Close();

	void SetInputSystem( InputSystem* input );
	InputSystem* GetInputSystem() const { return m_input; }
	void SetEventSystem( EventSystem* eventSystem );
	EventSystem* GetEventSystem() const { return m_event; }

	void BeginFrame();

	inline int GetClientWidth() const { return m_width; }
	inline int GetClientHeight() const { return m_height; }

public:
	void* m_hwnd = nullptr;
	int m_width;
	int m_height;
	InputSystem* m_input = nullptr;
	EventSystem* m_event = nullptr;
};