#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/Collision2D.hpp"

class Collider2D;
class Physics2D;

enum eSimulationMode
{
	RIGIDBODY_STATIC_MODE,		// objects do not apply forces or simulate.
	RIGIDBODY_KINEMATIC_MODE,	// objects do not apply forces, but will simulate.  
	RIGIDBODY_DYNAMIC_MODE		// objects apply forces and simulate
};

enum ePhysicsLayer : uint
{
	PHYSICS_LAYER_0,
	PHYSICS_LAYER_1,
	PHYSICS_LAYER_2,
	PHYSICS_LAYER_3,

	PHYSICS_LAYER_NUM
};

enum eUserType : uint
{
	USER_TYPE_GAMEOBJECT,
};

class Rigidbody2D
{
	friend class Physics2D;

public:
	void Destroy();                             // mark self for destruction, and mark collider as destruction
	void TakeCollider( Collider2D* collider );  // takes ownership of a collider (destroying my current one if present)
	void SetPosition( Vec2 position );          // update my position, and my colliders world position
	void Translate( Vec2 translation );
	void ApplyImpulseAt( Vec2 worldPos, Vec2 impulse );
	void ApplyDragForce();
	void AddForce( Vec2 force );
	void AddDrag( float amount );
	void AddTorque( float torque ) { m_frameTorque += torque; }

	eSimulationMode GetSimulationMode() const { return m_simulationMode; }
	ePhysicsLayer	GetPhysicsLayer() const { return m_physicsLayer; }
	Collider2D*		GetCollider() const { return m_collider; }
	Vec2			GetVelocity() const { return m_velocity; }
	Vec2			GetVerletVelocity();
	Vec2			GetImpactVelocityAtPoint( Vec2 worldPos );
	float			GetRotationInRadian() const { return m_rotationInRadians; }
	float			GetAngularVelocity() const { return m_angularVelocity; }
	float			GetFrameTorque() const { return m_frameTorque; }
	float			GetMoment() const { return m_moment; }
	void*			GetUserData( uint type ) const { return (type == m_userDataType) ? m_userData : nullptr; }
	bool			IsEnablePhysics() const { return m_enablePhysics; }
	bool			IsTrigger() const { return m_isTrigger; }

	void SetSimulationMode( eSimulationMode simulationMode ) { m_simulationMode = simulationMode; }
	void SetVelocity( const Vec2& velocity );
	void SetEnablePhysics( bool enablePhysics ) { m_enablePhysics = enablePhysics; }
	void SetRotationInRadian( float rotationInRadians ) { m_rotationInRadians = rotationInRadians; }
	void SetAngularVelocity( float angularVelocity ) { m_angularVelocity = angularVelocity; }
	void SetMoment( float moment ) { m_moment = moment; }
	void SetAsTrigger( bool isTrigger ) { m_isTrigger = isTrigger; }
	void SetPhysicsLayer( ePhysicsLayer layer ) { m_physicsLayer = layer; }
	void SetUserData( uint type, void* data );

public:
	Physics2D*		m_system = nullptr;     // which scene created/owns this object
	Collider2D*		m_collider = nullptr;
	Vec2			m_velocity;
	Vec2			m_worldPosition;     // where in the world is this rigidbody
	Vec2			m_frameStartPosition;
	Vec2			m_frameForce;
	eSimulationMode m_simulationMode = RIGIDBODY_DYNAMIC_MODE;
	ePhysicsLayer	m_physicsLayer = PHYSICS_LAYER_0;

	bool			m_readyForDelete = false;
	bool			m_enablePhysics = true;
	bool			m_isTrigger = false;

	float			m_drag = 0.f;
	float			m_rotationInRadians = 0.f;
	float			m_angularVelocity = 0.f;
	float			m_frameTorque = 0.f;
	float			m_moment = 0.f;

	void* m_userData = nullptr;
	uint m_userDataType = 0;

	//--------------------------------------------------------------------------------------------------------------------------
	//													Overlap
	//--------------------------------------------------------------------------------------------------------------------------
	Delegate<Collision2D const&> OnOverlapEnter; // called on frames a contact happens, but it wasn't their the frame before
	Delegate<Collision2D const&> OnOverlapExit;  // called on frames where no contact happened but we had one the frame before
	Delegate<Collision2D const&> OnOverlapStay;  // called on frames a contact happens and it happened the frame before
	Delegate<Collision2D const&> OnCollision;   // called after collision has been resolved this fixed step

	//--------------------------------------------------------------------------------------------------------------------------
	//													Trigger
	//--------------------------------------------------------------------------------------------------------------------------
	Delegate<Collision2D const&> OnTriggerEnter;
	Delegate<Collision2D const&> OnTriggerExit;
	Delegate<Collision2D const&> OnTriggerStay;

private:
	~Rigidbody2D();           // assert the collider is already null 
};