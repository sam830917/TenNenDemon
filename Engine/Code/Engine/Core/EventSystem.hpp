#pragma once
#include "Engine/Core/NamedProperties.hpp"
#include <string>
#include <vector>

typedef unsigned int EntityID;
typedef void(*EventCallbackFunction)( NamedProperties& args );                // static void some_method_impl( NamedStrings& args )

struct EventSubsciption
{
	EventSubsciption( std::string eventName, std::string eventDescription, EventCallbackFunction callbackFunc, const std::string& inputValue );
	std::string m_eventName;
	std::string m_eventDescription;
	NamedProperties m_inputValue;
	EventCallbackFunction m_callbackFunc = nullptr;
};

#define COMMAND( name, description, inputVal ) \
   static void name##_impl( NamedProperties& args ); \
   static EventSubsciption name##_register( #name, description, name##_impl, inputVal ); \
   static void name##_impl( NamedProperties& args )

class EventSystem
{
public:
	EventSystem();
	~EventSystem();
	void SubscriberToEvent( const std::string& eventName, std::string eventDescription, EventCallbackFunction eventCallBackFunction );
	void FireEvent( const std::string& eventName );
	void FireEvent( const std::string& eventName, NamedProperties& args );
	void FireEventWithValue( const std::string& commandWithValue );

	void Unsubscriber( const std::string& eventName );

	std::vector<EventSubsciption*> GetEventForDevConsole() const;
	bool IsDevConsoleVisible( const std::string& command );

public:
	std::vector<EventSubsciption*> m_eventSubscriptions;
};