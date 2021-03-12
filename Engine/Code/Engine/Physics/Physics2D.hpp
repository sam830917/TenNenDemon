#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/Collision2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include <vector>

class Collider2D;
class DiscCollider2D;
class PolygonCollider2D;
class Clock;
class Timer;

class Physics2D
{
public:
	Physics2D();
	~Physics2D();

	void Startup();
	void BeginFrame();
	void Update( float deltaSeconds );      // nothing in A01, but eventually it is the update, collision detection, and collision response part
	void EndFrame();
	void CleanUpDestroyedObjects();

	void SimulateStep( float deltaSeconds );
	void ApplyEffectors( float deltaSeconds );
	void MoveRigidbodies( float deltaSeconds );
	void DetectCollisions();
	void ResolveCollisions();
	void ResolveCollision( Collision2D const&  col );
	void ApplyObjectsForce( float deltaSeconds );

	void CheckLastFrameCollisionExit();
	bool IsNewCollision( const Collision2D&  col );

	Rigidbody2D*		CreateRigidbody();
	DiscCollider2D*		CreateDiscCollider( Vec2 localPosition, float radius );
	PolygonCollider2D*	CreatePolygonCollider( Vec2 const* points, uint pointCount, bool isMakeConvexFromPointCloud = false );

	void DestroyRigidbody( Rigidbody2D* rb );
	void DestroyCollider( Collider2D* collider );

	float GetFixedDeltaTime() const { return m_fixedDeltaTime; }
	bool  HasCollisionBetweenLayers( ePhysicsLayer layerA, ePhysicsLayer layerB );

	void SetClock( Clock* clock ) { m_clock = clock; }
	void SetSceneGravity( float gravityAmount );
	void SetFixedDeltaTime( float frameTimeSeconds );
	void SetPhysicsLayer( ePhysicsLayer layerA, ePhysicsLayer layerB, bool isCollision );

public:
	// add members you may need to store these
	// storage for all rigidbodies
	// storage for all colliders
	// ...
	std::vector<Rigidbody2D*> m_rigidbodyList;
	std::vector<Collider2D*> m_colliderList;

	std::vector<Collision2D> m_frameCollisions;
	std::vector<Collision2D> m_lastFrameCollisions;
	Delegate<float> OnFixedUpdate;             // called once for every step of the physics system
	bool m_physicsCollisionMatrix[PHYSICS_LAYER_NUM * PHYSICS_LAYER_NUM];

	float m_gravityAmount = GRAVITY;
	Clock* m_clock = nullptr;
	Timer* m_stepTimer = nullptr;
	float m_fixedDeltaTime = 1.f / 120.f; // 120hz seconds per frame. 

	double m_accumulatedTime = 0.0f;
};