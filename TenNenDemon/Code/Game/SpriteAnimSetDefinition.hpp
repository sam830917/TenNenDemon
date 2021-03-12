#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include <map>
#include <string>

class SpriteAnimSetDefinition
{
public:
	SpriteAnimSetDefinition( const XmlElement* spriteAnimSetElement );
	~SpriteAnimSetDefinition();

public:
	std::map< std::string, SpriteAnimDefinition* > m_spriteAnims;

};