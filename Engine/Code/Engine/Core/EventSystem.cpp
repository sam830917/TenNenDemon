#include "EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"

static uint constexpr MAX_REGISTERED_EVENTS( 128 );
static EventSubsciption* g_registrarList[MAX_REGISTERED_EVENTS];
static uint g_registrarCount = 0;

EventSubsciption::EventSubsciption( std::string eventName, std::string eventDescription, EventCallbackFunction callbackFunc, const std::string& inputValue )
	:m_eventName( eventName ),
	m_eventDescription( eventDescription ),
	m_callbackFunc( callbackFunc )
{
	m_inputValue.PopulateForEventCommand( inputValue );
	g_registrarList[g_registrarCount] = this;
	++g_registrarCount;
}

EventSystem::EventSystem()
{
	for ( uint i = 0; i < g_registrarCount; i++ )
	{
		m_eventSubscriptions.push_back( g_registrarList[i] );
	}
}

EventSystem::~EventSystem()
{
}

void EventSystem::SubscriberToEvent( const std::string& eventName, std::string eventDescription, EventCallbackFunction eventCallBackFunction )
{
	m_eventSubscriptions.push_back( new EventSubsciption( eventName, eventDescription, eventCallBackFunction, "" ) );
}

void EventSystem::FireEvent( const std::string& eventName )
{
	for ( int eventIndex = 0;eventIndex < (int) m_eventSubscriptions.size();eventIndex++ )
	{
		if ( eventName == m_eventSubscriptions[eventIndex]->m_eventName )
		{
			m_eventSubscriptions[eventIndex]->m_callbackFunc( m_eventSubscriptions[eventIndex]->m_inputValue );
			break;
		}
	}
}

void EventSystem::FireEvent( const std::string& eventName, NamedProperties& args )
{
	for( int eventIndex = 0; eventIndex < (int)m_eventSubscriptions.size(); eventIndex++ )
	{
		if( eventName == m_eventSubscriptions[eventIndex]->m_eventName )
		{
			m_eventSubscriptions[eventIndex]->m_callbackFunc( args );
			break;
		}
	}
}

void EventSystem::FireEventWithValue( const std::string& commandWithValue )
{
	Strings commandLine = SplitStringOnDelimiter( commandWithValue, ' ' );

	if ( (int) commandWithValue.size() > 0 )
	{
		EventSubsciption* eventSub = nullptr;
		std::string eventName = commandLine[0];
		for( int eventIndex = 0; eventIndex < (int)m_eventSubscriptions.size(); eventIndex++ )
		{
			if( eventName == m_eventSubscriptions[eventIndex]->m_eventName )
			{
				eventSub = m_eventSubscriptions[eventIndex];
				break;
			}
		}

		if ( eventSub != nullptr )
		{
			eventSub->m_inputValue.ResetValue();
			for ( int i = 1; i < (int) commandLine.size(); i++ )
			{
				Strings commandVal = SplitStringOnDelimiter( commandLine[i], '=' );
				if ( (int) commandVal.size() > 1 )
				{
					if ( eventSub->m_inputValue.HasValue( commandVal[0] ) )
					{
						eventSub->m_inputValue.SetValue( commandVal[0], commandVal[1].c_str() );
					}
					else
					{
						g_theConsole->PrintString( Rgba8::RED, Stringf("No such value : %s", commandVal[0].c_str()) );
					}
				}
			}
			
			eventSub->m_callbackFunc( eventSub->m_inputValue );
		}
	}
}

void EventSystem::Unsubscriber( const std::string& eventName )
{
	for( int eventIndex = 0; eventIndex < (int)m_eventSubscriptions.size(); eventIndex++ )
	{
		if( eventName == m_eventSubscriptions[eventIndex]->m_eventName )
		{
			delete m_eventSubscriptions[eventIndex];
			m_eventSubscriptions[eventIndex] = nullptr;

			uint lastIndex = (uint)m_eventSubscriptions.size() - 1;
			m_eventSubscriptions[eventIndex] = m_eventSubscriptions[lastIndex];
			m_eventSubscriptions.pop_back();
			--eventIndex;
			break;
		}
	}
}

std::vector<EventSubsciption*> EventSystem::GetEventForDevConsole() const
{
	std::vector<EventSubsciption*> eventList;
	for( uint i = 0; i < g_registrarCount; i++ )
	{
		EventSubsciption& eventSub = *g_registrarList[i];

		eventList.push_back( &eventSub );
	}

	return eventList;
}

bool EventSystem::IsDevConsoleVisible( const std::string& command )
{
	for( uint i = 0; i < g_registrarCount; i++ )
	{
		EventSubsciption& eventSub = *g_registrarList[i];
		if ( eventSub.m_eventName == command )
		{
			return true;
		}
	}
	return false;
}
