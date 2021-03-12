#include "ActorDefinition.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/StringUtils.hpp"

STATIC std::map< std::string, ActorDefinition* > ActorDefinition::s_definitionMap;

STATIC void ActorDefinition::LoadDefinitions( const std::string& deinitionsXmlFilePath )
{
	XmlDocument mapXmlDoc;
	mapXmlDoc.LoadFile( &deinitionsXmlFilePath[0] );
	XmlElement* mapDefsElement = mapXmlDoc.RootElement();
	if( mapDefsElement == nullptr )
	{
		g_theConsole->PrintString( Rgba8::RED, "WARNING: Actor definitions file not found" );
		return;
	}
	XmlElement* actorDefElement = mapDefsElement->FirstChildElement();
	while( actorDefElement )
	{
		ActorDefinition* newMapDef = new ActorDefinition( *actorDefElement );
		s_definitionMap[newMapDef->m_name] = newMapDef;
		actorDefElement = actorDefElement->NextSiblingElement();
	}
}

STATIC ActorDefinition* ActorDefinition::GetDefinitions( const std::string& deinitionsName )
{
	if ( s_definitionMap.count( deinitionsName ) > 0 )
	{
		return s_definitionMap.at( deinitionsName );
	}
	return nullptr;
}

ActorDefinition::ActorDefinition( const XmlElement& actorDefElement )
	:EntityDefinition( actorDefElement )
{

}

ActorDefinition::~ActorDefinition()
{
}