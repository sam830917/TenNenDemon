#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/Collision2D.hpp"

class Physics2D;
class Rigidbody2D;

enum eCollider2DType
{
	COLLIDER2D_DISC,
	COLLIDER2D_POLYGON,

	NUM_COLLIDER_TYPES,
	COLLIDER_UNKNOWN
};

struct PhysicsMaterial
{
public:
	float restitution = 1.f;
	float friction = 0.1f;

public:
	void AddRestitution( float amount );
	void AddFriction( float amount );
};

// Interface for all Collider objects used with our Physics system
class Collider2D
{
	friend class Physics2D;

public: // Interface 
		// cache off the world shape representation of this object
		// taking into account the owning rigidbody (if no owner, local is world)
	virtual void UpdateWorldShape()                             = 0;

	// queries 
	virtual Vec2	GetClosestPoint( Vec2 pos ) const				= 0;
	virtual Vec2	GetCenterPoint() const							= 0;
	virtual bool	Contains( Vec2 pos ) const						= 0;
	virtual void	Destroy()										= 0;
	virtual Vec2	GetBottomPosition()								= 0;
	virtual float	GetCosmeticRadius()								= 0;
	virtual AABB2	GetWorldBounds()								= 0;
	virtual float	CalculateMoment( float mass )					= 0;
	virtual bool	Intersects( Collider2D const* other ) const;
	Manifold2		GetManifold( Collider2D const* other );
	float			GetBounceWith(Collider2D const* other) const;
	float			GetFrictionWith(Collider2D const* other) const;
	void			AddMass( float amount );

	float GetMass() const { return m_mass; }

	// debug helpers
	virtual void DebugRender( RenderContext* ctx, Rgba8 const& borderColor, Rgba8 const& fillColor ) = 0;

protected:
	virtual ~Collider2D(); // private - make sure this is virtual so correct deconstructor gets called

public: 
	// any helpers you want to add

public:
	eCollider2DType m_type = COLLIDER_UNKNOWN;                // keep track of the type - will help with collision later
	Physics2D* m_system;                   // system who created or destr
	Rigidbody2D* m_rigidbody = nullptr;    // owning rigidbody, used for calculating world shape
	bool m_readyForDelete = false;
	AABB2 m_worldBound;
	float m_mass = 1.f;
	PhysicsMaterial m_material;
};