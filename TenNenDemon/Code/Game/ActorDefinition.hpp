#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "EntityDefinition.hpp"
#include <map>
#include <string>

class ActorDefinition : public EntityDefinition
{
public:
	static std::map< std::string, ActorDefinition* >	s_definitionMap;
	static void LoadDefinitions( const std::string& deinitionsXmlFilePath );
	static ActorDefinition* GetDefinitions( const std::string& deinitionsName );

public:
	ActorDefinition( const XmlElement& actorDefElement );
	~ActorDefinition();

private:

};