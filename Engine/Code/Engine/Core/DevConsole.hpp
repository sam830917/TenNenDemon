#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include <vector>
#include <string>
#include <map>

class RenderContext;
class BitmapFont;
class Camera;
class SpriteAnimDefinition;
class Clock;

struct ColoredLine
{
	ColoredLine( const Rgba8& initialColor, const std::string& initialText ):color( initialColor ), text( initialText ) {};
	Rgba8 color;
	std::string text;
};

class DevConsole
{
public:
	DevConsole();
	void Startup();
	void BeginFrame();
	void Update( float deltaSeconds );
	void EndFrame();
	void Shutdown();
	
	void PrintString( const Rgba8& textColor, const std::string& devConsolePrintString );
	void Error( const std::string& printString );
	void Render( RenderContext& renderer, const Camera& camera, float lineHeight, BitmapFont* font = nullptr ) const;
	
	void ToggleOpenFull();
	void Close();
	void Open();
	bool IsOpen() const;

	void ClearCurrentInput();
	void PrintCommands();

	void SetCamera( Camera* camera ) { m_camera = camera; }

private:
	int GetCarrotPosition() const;
	bool SendCommand( const std::string& command );
	void UpdateSensitiveList();
	void ProcessInput();
	void DeleteSelectedInputText();
	std::string GetSelectedInputText() const;
	bool IsSelectingInputText() const { return m_carrotPosition != m_carrotSelectedPosition; };

public:
	SpriteAnimDefinition* m_backgroundAnim = nullptr;
	AABB2 m_backgroundAnimBox;
	Rgba8 m_backgroundColor = Rgba8( 0, 0, 50, 150 );

private:
	bool m_open = false;
	std::vector<ColoredLine> m_coloredLineHistory;
	Rgba8 m_defaultBackgroundColor = Rgba8::BLACK;

	std::string m_currentInput;
	std::vector<std::string> m_commandHistory;
	unsigned int m_commandHistoryIdx = 0;
	int m_carrotPosition = 0;
	int m_carrotSelectedPosition = 0;

	std::vector<std::string> m_sensitiveStrList;
	unsigned int m_sensitiveSelectIdx = 0;
	unsigned int m_scollingRow = 0;

	Clock* m_clock = nullptr;
	Camera* m_camera = nullptr;
	bool m_isMouseSelecting = false;

	int m_maxTextLengthPerLine = 95;
};