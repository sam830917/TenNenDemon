#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "SpriteAnimSetDefinition.hpp"
#include "GameCommon.hpp"

enum eEntityType
{
	ENTITY_TYPE_UNKNOWN,
	ENTITY_TYPE_ACTOR,
	ENTITY_TYPE_LAVA,
	ENTITY_TYPE_BREAKABLE_OBJECT,
	ENTITY_TYPE_UNBREAKABLE_OBJECT,
	ENTITY_TYPE_PET,
	ENTITY_TYPE_FIREBALL,
	ENTITY_TYPE_PLAYER,

	NUM_ENTITY_TYPES
};
enum Faction
{
	FACTION_GOOD,
	FACTION_EVIL,
	FACTION_NEUTRAL
};

class EntityDefinition
{
public:
	explicit EntityDefinition( const XmlElement& definitionXmlElement );
	virtual ~EntityDefinition();

public:
	std::string					m_name;
	std::string					m_faction;
	float						m_physicsRadius = DEFAULT_PHYSICS_RADIUS;
	float						m_drawRadius = DEFAULT_DRAW_RADIUS;
	AABB2						m_localDrawBounds = AABB2( -DEFAULT_DRAW_RADIUS, -DEFAULT_DRAW_RADIUS, DEFAULT_DRAW_RADIUS, DEFAULT_DRAW_RADIUS );
	SpriteAnimSetDefinition*	m_animSetDef = nullptr;
};