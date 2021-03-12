#pragma once
#include "Entity.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>

class EnvironmentEntityDefinition;
class Emitter;

class Lava : public Entity
{
public:
	Lava( Game* theGame, Map* theMap, const Vec2& pos, EnvironmentEntityDefinition* def );
	~Lava();
	virtual void Update( float deltaSeconds );
	virtual void Render() const;

	void DebugModeRender() const;
	bool IsFilled() const { return m_isFilled;}
	void FillLava() { m_isFilled = true; }

private:
	EnvironmentEntityDefinition* m_environmentEntityDef = nullptr;
	bool m_isFilled = false;
	Emitter* m_emitter = nullptr;

};