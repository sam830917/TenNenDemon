#pragma once
#include "Entity.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>

class Player : public Entity
{
public:
	Player( Game* theGame, Map* theMap, const Vec2& pos, const std::string& name );
	~Player();
	virtual void Update( float deltaSeconds );
	virtual void Render() const;

	void DebugModeRender() const;
	void HandlePlayerInput( float deltaSeconds );

private:

};