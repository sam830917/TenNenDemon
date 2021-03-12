#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/EntityDefinition.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include <vector>
#include <string>

class Map;
class Timer;

class Entity
{
public:
	Entity( Game* theGame, Map* theMap, const Vec2& pos );
	virtual ~Entity();

	virtual void Update( float deltaSeconds );
	virtual void Render() const;
	virtual void Die();
	virtual void TakeDamage( int damagePoint );
	virtual void Move( int targetIndexX, int targetIndexY );

	virtual bool IsOffscreen() const;
	virtual bool IsDead() const;

	float GetSpeed() const { return m_speed; }

public:
	Game* m_game = nullptr;
	Map* m_map = nullptr;

	std::string			m_name = "UNNAMED";
	Timer*				m_timer = nullptr;
	eEntityType			m_entityType = ENTITY_TYPE_UNKNOWN;
	int					m_entityID = 0;
	EntityDefinition*	m_entityDef = nullptr;
	bool				m_isDead = false;
	bool				m_isReadyToBeDeleted = false;
	bool				m_isMoving = false;
	Vec2				m_position = Vec2::ZERO;
	float				m_speed = 30.f;
	IntVec2				m_currentPositionIndex = IntVec2::ZERO;
	IntVec2				m_targetPositionIndex = IntVec2::ZERO;
};

typedef std::vector<Entity*> EntityList;