#include "EntityDefinition.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"

EntityDefinition::EntityDefinition( const XmlElement& definitionXmlElement )
{
	m_name = ParseXmlAttribute( definitionXmlElement, "name", m_name );

	// Size
	const XmlElement* sizeElement = definitionXmlElement.FirstChildElement( "Size" );
	m_physicsRadius = ParseXmlAttribute( *sizeElement, "physicsRadius", m_physicsRadius );
	m_localDrawBounds = ParseXmlAttribute( *sizeElement, "localDrawBounds", m_localDrawBounds );

	// SpriteAnimSet
	const XmlElement* spriteAnimSetElement = definitionXmlElement.FirstChildElement( "SpriteAnimSet" );
	m_animSetDef = new SpriteAnimSetDefinition( spriteAnimSetElement );
}

EntityDefinition::~EntityDefinition()
{
}
