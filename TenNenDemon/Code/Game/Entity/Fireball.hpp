#pragma once
#include "Entity.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>

class Fireball : public Entity
{
public:
	Fireball( Game* theGame, Map* theMap, const Vec2& pos, const std::string& name );
	~Fireball();
	virtual void Update( float deltaSeconds );
	virtual void Render() const;

	void DebugModeRender() const;

	void SetMovingDirection( const IntVec2& movingDirection );

public:
	IntVec2 m_movingDirection = IntVec2( 1, 0 );
	std::string m_spriteName = "East";

};