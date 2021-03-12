#include "Entity.hpp"
#include "Game/Map.hpp"
#include "Game/MapDefinition.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Math/MathUtils.hpp"

Entity::Entity( Game* theGame, Map* theMap, const Vec2& pos )
	:m_game(theGame),
	m_map(theMap),
	m_position( pos )
{
	m_timer = new Timer( m_map->m_mapClock );
}

Entity::~Entity()
{
}

void Entity::Update( float deltaSeconds )
{
	if ( !m_isMoving )
	{
		return;
	}
	Vec2 targetPosition = m_map->GetTileCenterPosition( m_targetPositionIndex );
	Vec2 originPosition = m_map->GetTileCenterPosition( m_currentPositionIndex );
	Vec2 direction = targetPosition - originPosition;
	m_position += direction * m_speed * deltaSeconds;
	AABB2 bounds = m_map->m_tiles[m_map->GetTileIndexForTileCoords( m_targetPositionIndex )].GetBounds();
	if( IsPointInsideAABB2D( m_position, bounds ) || m_timer->HasElapsed() )
	{
		m_isMoving = false;
		m_currentPositionIndex = m_targetPositionIndex;
		m_position = targetPosition;
	}
}

void Entity::Render() const
{
}

void Entity::Die()
{
	m_isDead = true;
}

void Entity::TakeDamage( int damagePoint )
{
	UNUSED( damagePoint );
}

void Entity::Move( int targetIndexX, int targetIndexY )
{
	if ( m_map->IsOutOfBounds( targetIndexX, targetIndexY ) )
	{
		return;
	}

	m_timer->SetSeconds( m_map->m_mapClock, 0.05f );
	m_isMoving = true;
	m_targetPositionIndex = IntVec2( targetIndexX, targetIndexY );
}

bool Entity::IsOffscreen() const
{
	return false;
}

bool Entity::IsDead() const
{
	return m_isDead;
}

