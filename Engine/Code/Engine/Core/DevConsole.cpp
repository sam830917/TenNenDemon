#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Platform/WindowUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>

extern BitmapFont* g_theFont;
extern DevConsole* g_theConsole;
extern EventSystem* g_theEventSystem;
extern InputSystem* g_theInput;

DevConsole::DevConsole()
{
}

void DevConsole::Startup()
{
	m_clock = new Clock();
}

void DevConsole::BeginFrame()
{
}

void DevConsole::Update( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	if( !IsOpen() )
		return;

	if ( g_theInput->GetMouseWheelScrollAmount() > 0.f )
	{
		if ( (int)m_coloredLineHistory.size() - m_scollingRow - 1 > 0 )
		{
			m_scollingRow++;
		}
	}
	if( g_theInput->GetMouseWheelScrollAmount() < 0.f )
	{
		if( m_scollingRow != 0 )
		{
			m_scollingRow--;
		}
	}

	if ( g_theInput->WasMouseButtonJustPressed( LEFT_BUTTON ) )
	{
		Vec2 mousePos = g_theInput->GetMouseNormalizedClientPos();
		mousePos = m_camera->ClientToWorldPosition( mousePos, 0.f );

		Vec2 inputTextDimension = g_theFont->GetDimensionsForText2D( 1.f, m_currentInput );
		AABB2 inputTextBox = AABB2( Vec2::ZERO, inputTextDimension );
		inputTextBox.Translate( m_camera->GetOrthoBottomLeft() );
		AABB2 inputBarBox = AABB2( Vec2( inputTextBox.maxs.x, inputTextBox.maxs.y - 1.f ), Vec2( m_camera->GetOrthoTopRight().x, 1.f ) );

		if ( IsPointInsideAABB2D( mousePos, inputTextBox ) )
		{
			Vec2 mouseNormalPos = mousePos - m_camera->GetOrthoBottomLeft();
			int gotoIndex = (int) Round( mouseNormalPos.x );
			m_carrotPosition = (int) m_currentInput.size() - gotoIndex;
			m_carrotSelectedPosition = m_carrotPosition;
			m_isMouseSelecting = true;
		}
		else if ( IsPointInsideAABB2D( mousePos, inputBarBox ) )
		{
			m_carrotPosition = 0;
			m_carrotSelectedPosition = 0;
			m_isMouseSelecting = true;
		}
	}
	if ( g_theInput->IsMouseButtonPressed( LEFT_BUTTON ) )
	{
		Vec2 mousePos = g_theInput->GetMouseNormalizedClientPos();
		mousePos = m_camera->ClientToWorldPosition( mousePos, 0.f );
		Vec2 inputTextDimension = g_theFont->GetDimensionsForText2D( 1.f, m_currentInput );
		AABB2 inputTextBox = AABB2( Vec2::ZERO, inputTextDimension );
		inputTextBox.Translate( m_camera->GetOrthoBottomLeft() );
		AABB2 inputBarBox = AABB2( Vec2( inputTextBox.maxs.x, inputTextBox.maxs.y - 1.f ), Vec2( m_camera->GetOrthoTopRight().x, 1.f ) );

		if( IsPointInsideAABB2D( mousePos, inputTextBox ) || m_isMouseSelecting )
		{
			Vec2 mouseNormalPos = mousePos - m_camera->GetOrthoBottomLeft();
			int gotoIndex = (int)Round( mouseNormalPos.x );
			m_carrotSelectedPosition = (int)m_currentInput.size() - gotoIndex;
		}
		else if( IsPointInsideAABB2D( mousePos, inputBarBox ) )
		{
			m_carrotSelectedPosition = 0;
		}
	}
	if( g_theInput->WasMouseButtonJustReleased( LEFT_BUTTON ) )
	{
		m_isMouseSelecting = false;
	}

	ProcessInput();
}

void DevConsole::EndFrame()
{
}

void DevConsole::Shutdown()
{
}

void DevConsole::PrintString( const Rgba8& textColor, const std::string& devConsolePrintString )
{
	if ( devConsolePrintString.size() == 0 )
		return;

	float cameraWidth = m_camera->m_outputSize.x;
	float strWidth = g_theFont->GetDimensionsForText2D( 1.f, devConsolePrintString ).x;
	if ( cameraWidth >= strWidth )
	{
		ColoredLine newLine( textColor, devConsolePrintString );
		m_coloredLineHistory.push_back( newLine );
	}
	else
	{
		std::string remainingStr = devConsolePrintString;
		while( (int) remainingStr.size() > m_maxTextLengthPerLine )
		{
			std::string line = remainingStr.substr( 0, m_maxTextLengthPerLine );
			ColoredLine newLine( textColor, line );
			m_coloredLineHistory.push_back( newLine );

			remainingStr = remainingStr.substr( m_maxTextLengthPerLine, remainingStr.size() - 1 );
		}
		ColoredLine newLine( textColor, remainingStr );
		m_coloredLineHistory.push_back( newLine );
	}
}

void DevConsole::Error( const std::string& printString )
{
	Open();
	PrintString( Rgba8::RED, printString );
}

void DevConsole::Render( RenderContext& renderer, const Camera& camera, float lineHeight, BitmapFont* font ) const
{
	if ( !IsOpen() )
		return;

	if( font == nullptr )
		font = g_theFont;

	AABB2 background = AABB2( camera.GetOrthoBottomLeft(), camera.GetOrthoTopRight() );
	renderer.BindTexture( nullptr );
	renderer.DrawAABB2D( background, m_backgroundColor );
	// show background image
	if( m_backgroundAnim )
	{
		float seconds = (float) m_clock->GetTotalElapsedSeconds();
		SpriteDefinition sd = m_backgroundAnim->GetSpriteDefAtTime( seconds );
		Vec2 uvAtMins, uvAtMaxs;
		sd.GetUVs( uvAtMins, uvAtMaxs );
		renderer.BindTexture( &m_backgroundAnim->GetSpriteDefAtTime( seconds ).GetSpriteSheet().GetTexture() );
		renderer.DrawAABB2D( m_backgroundAnimBox, AABB2( uvAtMins, uvAtMaxs ), Rgba8::WHITE );
	}
	// input frame
	AABB2 inputBox = AABB2( camera.GetOrthoBottomLeft(), Vec2( camera.GetOrthoTopRight().x, camera.GetOrthoBottomLeft().y + lineHeight ) );
	renderer.BindTexture( nullptr );
	renderer.DrawAABB2D( inputBox, Rgba8::BLACK );

	float linePositionY = camera.GetOrthoBottomLeft().y + lineHeight;
	float maxHeight = camera.GetOrthoTopRight().y;
	std::vector<Vertex_PCU> textVerts;
	if ( (int)m_coloredLineHistory.size() > 0 )
	{
		for( int index = (int)m_coloredLineHistory.size() - m_scollingRow - 1; index >= 0; index-- )
		{
			if( linePositionY >= maxHeight )
				break;

			std::string historyLine = m_coloredLineHistory[index].text;
			historyLine.insert( 0, "- " );
			font->AddVertsForText2D( textVerts, Vec2( camera.GetOrthoBottomLeft().x, linePositionY ), lineHeight, historyLine, m_coloredLineHistory[index].color );
			linePositionY += lineHeight;
		}
	}
	// print sensitive string
	if( (int) m_sensitiveStrList.size() != 0 && !IsSelectingInputText() )
	{
		font->AddVertsForText2D( textVerts, camera.GetOrthoBottomLeft(), lineHeight, m_sensitiveStrList[m_sensitiveSelectIdx], Rgba8( 255, 255, 255, 100 ) );
	}
	// print input string
	font->AddVertsForText2D( textVerts, camera.GetOrthoBottomLeft(), lineHeight, m_currentInput, Rgba8::WHITE );

	renderer.BindShader( renderer.CreateOrGetShader( "Data/Shaders/Outline.hlsl" ) );
	renderer.BindTexture( font->GetTexture() );
	renderer.DrawVertexArray( textVerts );

	// print multiple sensitive words box
	if ( (int) m_sensitiveStrList.size() > 1 && !IsSelectingInputText() )
	{
		const float boxHeight = lineHeight * 1.5f;
		linePositionY = camera.GetOrthoBottomLeft().y + lineHeight + boxHeight * ((int) m_sensitiveStrList.size() - 1);
		std::vector<Vertex_PCU> sensitiveTextVerts;
		float boxWidth = 10.f;

		// set box width
		for ( unsigned int i = 0; i < (int) m_sensitiveStrList.size(); i++ )
		{
			Vec2 textDimension = font->GetDimensionsForText2D( lineHeight, m_sensitiveStrList[i] );

			if ( textDimension.x > boxWidth )
			{
				boxWidth = textDimension.x;
			}
		}

		float posX = camera.GetOrthoBottomLeft().x + font->GetDimensionsForText2D( lineHeight, m_currentInput.substr( 0, GetCarrotPosition() ) ).x;
		for ( unsigned int i = 0; i < (int) m_sensitiveStrList.size(); i++ )
		{
			AABB2 box = AABB2( Vec2( posX, linePositionY ), Vec2( posX + lineHeight + boxWidth, linePositionY + boxHeight ) );
			renderer.BindTexture( nullptr );
			if ( i == m_sensitiveSelectIdx )
			{
				renderer.DrawAABB2D( box, Rgba8( 192, 192, 192 ) );
				font->AddVertsForTextInBox2D( sensitiveTextVerts, box, lineHeight, m_sensitiveStrList[i], Rgba8::BLACK, 1.f, ALIGN_MIDDLE_LEFT );
			}
			else
			{
				renderer.DrawAABB2D( box, Rgba8( 63, 63, 63 ) );
				font->AddVertsForTextInBox2D( sensitiveTextVerts, box, lineHeight, m_sensitiveStrList[i], Rgba8::WHITE, 1.f, ALIGN_MIDDLE_LEFT );
			}
			renderer.BindTexture( font->GetTexture() );
			renderer.DrawVertexArray( sensitiveTextVerts );

			linePositionY -= boxHeight;
		}
	}

	// select character
	if( IsSelectingInputText() )
	{
		int selectIdxA = GetCarrotPosition();
		int selectIdxB = (int)m_currentInput.size() - m_carrotSelectedPosition;
		Vec2 selectPosA = font->GetDimensionsForText2D( lineHeight, m_currentInput.substr( 0, selectIdxA ) );
		selectPosA = camera.GetOrthoBottomLeft() + Vec2( selectPosA.x, 0.f );
		Vec2 selectPosB = font->GetDimensionsForText2D( lineHeight, m_currentInput.substr( 0, selectIdxB ) );
		selectPosB = camera.GetOrthoBottomLeft() + Vec2( selectPosB.x, 0.f );

		AABB2 selectedBox;
		if( selectPosA.x > selectPosB.x )
		{
			selectedBox = AABB2( selectPosB, Vec2( selectPosA.x, camera.GetOrthoBottomLeft().y + lineHeight ) );
		}
		else
		{
			selectedBox = AABB2( selectPosA, Vec2( selectPosB.x, camera.GetOrthoBottomLeft().y + lineHeight ) );
		}
		renderer.BindTexture( nullptr );
		renderer.DrawAABB2D( selectedBox, Rgba8::YELLOW );

		std::string selectStr;
		std::vector<Vertex_PCU> selectedTextVerts;
		if( selectPosA.x > selectPosB.x )
		{
			selectStr = m_currentInput.substr( selectIdxB, selectIdxA - selectIdxB );
			font->AddVertsForText2D( selectedTextVerts, selectPosB, lineHeight, selectStr, Rgba8::BLACK );
		}
		else
		{
			selectStr = m_currentInput.substr( selectIdxA, selectIdxB - selectIdxA );
			font->AddVertsForText2D( selectedTextVerts, selectPosA, lineHeight, selectStr, Rgba8::BLACK );
		}
		renderer.BindTexture( font->GetTexture() );
		renderer.DrawVertexArray( selectedTextVerts );
	}

	// caret
	Vec2 inputTextPos = font->GetDimensionsForText2D( lineHeight, m_currentInput.substr( 0, GetCarrotPosition() ) );
	inputTextPos = camera.GetOrthoBottomLeft() + Vec2( inputTextPos.x, 0.f );
	Rgba8 caretColor = Rgba8::WHITE;
	double seconds = m_clock->GetTotalElapsedSeconds() * 2.0;
	int iseconds = (int)seconds;
	if ( iseconds % 2 == 0 )
	{
		caretColor.a = 0;
	}
	renderer.BindTexture( nullptr );
	renderer.DrawLine( inputTextPos, inputTextPos + Vec2( 0.f, lineHeight ), caretColor, 0.2f );
}

void DevConsole::ToggleOpenFull()
{
	ClearCurrentInput();
	if( g_theConsole->IsOpen() )
	{
		Close();
	}
	else
	{
		Open();
	}
}

void DevConsole::Close()
{
	m_open = false;
	m_isMouseSelecting = false;
	g_theInput->PopMouseOptions();
}

void DevConsole::Open()
{
	m_open = true;

	g_theInput->PushMouseOptions( MOUSE_MODE_ABSOLUTE, /*visible*/ true, /*clipped*/ false );
}

bool DevConsole::IsOpen() const
{
	return m_open;
}

void DevConsole::ClearCurrentInput()
{
	m_currentInput.clear();
	m_carrotPosition = 0;
	m_carrotSelectedPosition = 0;
}

void DevConsole::PrintCommands()
{
	int longestSizeChar = 0;
	std::vector<EventSubsciption*> eventList = g_theEventSystem->GetEventForDevConsole();
	for( int i = 0; i < (int) eventList.size(); i++ )
	{
		EventSubsciption& eventSub = *eventList[i];
		if( longestSizeChar < (int)eventSub.m_eventName.size() )
		{
			longestSizeChar = (int)eventSub.m_eventName.size();
		}
	}

	for( int eventIdx = 0; eventIdx < (int) eventList.size(); eventIdx++ )
	{
		EventSubsciption& eventSub = *eventList[eventIdx];
		
		std::string commandLine = eventSub.m_eventName;
		for( int i = 0; i < longestSizeChar - (int)eventSub.m_eventName.size(); i++ )
		{
			commandLine.insert( commandLine.size(), " " );
		}
		PrintString( Rgba8::WHITE, Stringf( "  %s    %s", commandLine.c_str(), eventSub.m_eventDescription.c_str() ) );
	}

}

int DevConsole::GetCarrotPosition() const
{
	return (int) m_currentInput.size() - m_carrotPosition;
}

bool DevConsole::SendCommand( const std::string& command )
{
	if ( (int) command.size() == 0 )
		return false;

	Strings commandLine = SplitStringOnDelimiter( command, ' ' );
	if ( (int) commandLine.size() > 0 && g_theEventSystem->IsDevConsoleVisible( commandLine[0] ) )
	{
		PrintString( Rgba8::WHITE, command );
		g_theEventSystem->FireEventWithValue( command );
		return true;
	}
	PrintString( Rgba8::YELLOW, Stringf( "Invalid Command : '%s'", command.c_str() ) );

	return false;
}

void DevConsole::UpdateSensitiveList()
{
	m_sensitiveStrList.clear();
	m_sensitiveSelectIdx = 0;
	if ( (int) m_currentInput.size() == 0 )
		return;

	std::vector<EventSubsciption*> eventList = g_theEventSystem->GetEventForDevConsole();
	for( int i = 0; i < (int) eventList.size(); i++ )
	{
		EventSubsciption& eventSub = *eventList[i];

		if ( eventSub.m_eventName.find( m_currentInput ) == 0 && eventSub.m_eventName != m_currentInput )
		{
			m_sensitiveStrList.push_back( eventSub.m_eventName );
		}
	}
}

void DevConsole::ProcessInput()
{
	char c;
	while( g_theInput->PopCharacter( &c ) )
	{
		m_isMouseSelecting = false;
		// only store the character that font texture supports
		if ( 32 <= c && c <= 126 )
		{
			if ( VK_SPACE == c && (int) m_sensitiveStrList.size() != 0 && !IsSelectingInputText() )
			{
				m_currentInput = m_sensitiveStrList[m_sensitiveSelectIdx];
				m_sensitiveStrList.clear();
			}
			else
			{
				DeleteSelectedInputText();
				m_currentInput.insert( GetCarrotPosition(), 1/*size_type*/, c );
				UpdateSensitiveList();
			}
		}
		// backspace
		else if ( VK_BACK == c )
		{
			if( m_carrotPosition != m_carrotSelectedPosition )
			{
				DeleteSelectedInputText();
			}
			else if ( GetCarrotPosition() != 0 )
			{
				m_currentInput = m_currentInput.erase( GetCarrotPosition() - 1, 1 );
			}
			UpdateSensitiveList();
		}
		// enter
		else if ( VK_RETURN == c )
		{
			SendCommand( m_currentInput );
			if ( (int) m_commandHistory.size() == 0 || m_commandHistory.back() != m_currentInput )
			{
				m_commandHistory.push_back( m_currentInput );
			}
			ClearCurrentInput();
			UpdateSensitiveList();
			m_commandHistoryIdx = 0;
			m_scollingRow = 0;
		}
		else if ( KEY_COPY == c )
		{
			std::string selectedText = GetSelectedInputText();
			SetClipboard( selectedText );
		}
		else if( KEY_CUT == c )
		{
			std::string selectedText = GetSelectedInputText();
			SetClipboard( selectedText );

			DeleteSelectedInputText();
			UpdateSensitiveList();
		}
		else if( KEY_PASTE == c )
		{
			std::string str = GetClipboard();
			if ( (int) str.size() > 0 )
			{
				DeleteSelectedInputText();
				m_currentInput.insert( GetCarrotPosition(), str );
			}
			UpdateSensitiveList();
		}
		else if( KEY_CTRL_A == c )
		{
			m_carrotPosition = 0;
			m_carrotSelectedPosition = (int) m_currentInput.size();
		}
	}

	// Remove a char
	if( g_theInput->WasKeyJustPressed( VK_DELETE ) )
	{
		if( m_carrotPosition != m_carrotSelectedPosition )
		{
			DeleteSelectedInputText();
		}
		else if( GetCarrotPosition() != (int)m_currentInput.size() )
		{
			m_currentInput = m_currentInput.erase( GetCarrotPosition(), 1 );
			m_carrotPosition--;
			m_carrotSelectedPosition = m_carrotPosition;
		}
		UpdateSensitiveList();
	}
	// Move Caret
	if( g_theInput->IsKeyPressed( VK_CONTROL ) )
	{
		if( g_theInput->WasKeyJustPressed( VK_LEFT ) && (int)m_currentInput.size() > m_carrotPosition )
		{
			std::string inputStr = m_currentInput.substr( 0, (int)m_currentInput.size() - m_carrotPosition - 1 );
			int spaceIndex = (int)inputStr.rfind(' ');
			if ( spaceIndex != -1 )
			{
				m_carrotPosition = (int)m_currentInput.size() - spaceIndex - 1;
				m_carrotSelectedPosition = m_carrotPosition;
			}
			else
			{
				m_carrotPosition = (int)m_currentInput.size();
				m_carrotSelectedPosition = m_carrotPosition;
			}
		}
		if( g_theInput->WasKeyJustPressed( VK_RIGHT ) && m_carrotPosition > 0 )
		{
			std::string inputStr = m_currentInput.substr( (int)m_currentInput.size() - m_carrotPosition, (int)m_currentInput.size() );
			int spaceIndex = (int)inputStr.find( ' ' );
			if ( spaceIndex != -1 )
			{
				int frontSize = (int)(m_currentInput.size() - inputStr.size());
				m_carrotPosition = (int)m_currentInput.size() - (frontSize + spaceIndex) - 1;
				m_carrotSelectedPosition = m_carrotPosition;
			}
			else
			{
				m_carrotPosition = 0;
				m_carrotSelectedPosition = m_carrotPosition;
			}
		}
	}
	else
	{
		if( g_theInput->WasKeyJustPressed( VK_LEFT ) && (int)m_currentInput.size() > m_carrotPosition )
		{
			m_carrotPosition++;
			if( !g_theInput->IsKeyPressed( VK_MENU ) )
			{
				m_carrotSelectedPosition = m_carrotPosition;
			}
		}
		if( g_theInput->WasKeyJustPressed( VK_RIGHT ) && m_carrotPosition > 0 )
		{
			m_carrotPosition--;
			if( !g_theInput->IsKeyPressed( VK_MENU ) )
			{
				m_carrotSelectedPosition = m_carrotPosition;
			}
		}
	}
	if( g_theInput->WasKeyJustPressed( VK_END ) )
	{
		m_carrotPosition = 0;
		m_carrotSelectedPosition = m_carrotPosition;
	}
	if( g_theInput->WasKeyJustPressed( VK_HOME ) )
	{
		m_carrotPosition = (int)m_currentInput.size();
		m_carrotSelectedPosition = m_carrotPosition;
	}

	if ( (int) m_sensitiveStrList.size() > 1 && !IsSelectingInputText() )
	{
		if( g_theInput->WasKeyJustPressed( VK_UP ) && m_sensitiveSelectIdx > 0 )
		{
			m_sensitiveSelectIdx--;
		}
		if( g_theInput->WasKeyJustPressed( VK_DOWN ) && (unsigned int) m_sensitiveStrList.size() - 1 != m_sensitiveSelectIdx )
		{
			m_sensitiveSelectIdx++;
		}
	}
	else
	{
		if( (int)m_commandHistory.size() != 0 )
		{
			unsigned int commandHistorySize = (unsigned int)m_commandHistory.size();
			if( g_theInput->WasKeyJustPressed( VK_UP ) && m_commandHistoryIdx != commandHistorySize )
			{
				m_commandHistoryIdx++;
				m_currentInput = m_commandHistory[commandHistorySize - m_commandHistoryIdx];
				UpdateSensitiveList();
			}
			if( g_theInput->WasKeyJustPressed( VK_DOWN ) && m_commandHistoryIdx != 0 )
			{
				m_commandHistoryIdx--;
				if( m_commandHistoryIdx == 0 )
				{
					m_currentInput = "";
				}
				else
				{
					m_currentInput = m_commandHistory[commandHistorySize - m_commandHistoryIdx];
				}
				UpdateSensitiveList();
			}
		}
	}
}

void DevConsole::DeleteSelectedInputText()
{
	if( m_carrotPosition == m_carrotSelectedPosition )
	{
		return;
	}

	int selectIdxA = GetCarrotPosition();
	int selectIdxB = (int)m_currentInput.size() - m_carrotSelectedPosition;
	if( selectIdxA > selectIdxB )
	{
		m_currentInput = m_currentInput.erase( selectIdxB, selectIdxA - selectIdxB );
		m_carrotSelectedPosition = m_carrotPosition;
		m_carrotSelectedPosition < 0 ? m_carrotSelectedPosition = 0 : true;
	}
	else
	{
		m_currentInput = m_currentInput.erase( selectIdxA, selectIdxB - selectIdxA );
		m_carrotPosition = m_carrotSelectedPosition;
		m_carrotPosition < 0 ? m_carrotPosition = 0 : true;
	}
}

std::string DevConsole::GetSelectedInputText() const
{
	if( m_carrotPosition == m_carrotSelectedPosition )
	{
		return "";
	}

	int selectIdxA = GetCarrotPosition();
	int selectIdxB = (int)m_currentInput.size() - m_carrotSelectedPosition;
	std::string selectStr;
	if( selectIdxA > selectIdxB )
	{
		selectStr = m_currentInput.substr( selectIdxB, selectIdxA - selectIdxB );
	}
	else
	{
		selectStr = m_currentInput.substr( selectIdxA, selectIdxB - selectIdxA );
	}
	return selectStr;
}

COMMAND ( help, "Lists all exposed commands.", "" )
{
	UNUSED( args );
	g_theConsole->PrintCommands();
}

COMMAND ( test, "Do nothing just for testing.", "" )
{
	UNUSED( args );
}

COMMAND( test2, "Another do nothing command.", "" )
{
	UNUSED( args );
}

COMMAND( change_console_color, "Change console color. e.g. change_console_color color=255,255,255", "color" )
{
	Rgba8 color = g_theConsole->m_backgroundColor;
	color = args.GetValue( "color", color );

	g_theConsole->m_backgroundColor = color;
}