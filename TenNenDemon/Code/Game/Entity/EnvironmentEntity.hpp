#pragma once
#include "Game/Entity/Entity.hpp"
#include <string>

class EnvironmentEntityDefinition;

class EnvironmentEntity : public Entity
{
public:
	EnvironmentEntity( Game* theGame, Map* theMap, const Vec2& pos, EnvironmentEntityDefinition* def );
	~EnvironmentEntity();
	virtual void Update( float deltaSeconds );
	virtual void Render() const;

	void DebugModeRender() const;

private:
	EnvironmentEntityDefinition* m_environmentEntityDef = nullptr;

};