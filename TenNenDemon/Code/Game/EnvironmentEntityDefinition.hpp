#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Math/IntVec2.hpp"

class EnvironmentEntityDefinition
{
public:
	EnvironmentEntityDefinition( const std::string& name, const IntVec2& startIndex, eEntityType entityType );
	~EnvironmentEntityDefinition();

public:
	IntVec2 m_startIndex = IntVec2::ZERO;
	eEntityType m_entityType = ENTITY_TYPE_BREAKABLE_OBJECT;
	ActorDefinition* m_actorDef = nullptr;

};