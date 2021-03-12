#pragma once
#include "Entity.hpp"
#include "Game/ActorDefinition.hpp"
#include <string>

class Timer;

enum eDirection
{
	DIRECTION_UP,
	DIRECTION_LEFT,
	DIRECTION_RIGHT,
	DIRECTION_DOWN,
};

class Actor : public Entity
{
public:
	Actor( Game* theGame, Map* theMap, const Vec2& pos, const std::string& name, eEntityType entityType );
	~Actor();
	virtual void Update( float deltaSeconds );
	virtual void Render() const;

	void DebugModeRender() const;
	void ChangePlayerSprite();

	void SetControllerID( int ID ) { m_controllerID = ID; }

private:
	void UpdateFromKeyBoard( float deltaSeconds );

private:
	int	m_controllerID = 0;

	bool m_isMoving = false;
	bool m_isAttacking = false;

	std::string m_currentSpriteName = "IdleSouth";
	eDirection m_direction = DIRECTION_DOWN;

};