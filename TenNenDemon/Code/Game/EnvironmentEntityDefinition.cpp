#include "Game/EnvironmentEntityDefinition.hpp"

EnvironmentEntityDefinition::EnvironmentEntityDefinition( const std::string& name, const IntVec2& startIndex, eEntityType entityType )
{
	m_startIndex = startIndex;
	m_actorDef = ActorDefinition::GetDefinitions( name );
	m_entityType = entityType;
}

EnvironmentEntityDefinition::~EnvironmentEntityDefinition()
{
}
