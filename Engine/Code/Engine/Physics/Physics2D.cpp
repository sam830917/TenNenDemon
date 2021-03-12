#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Renderer/DebugRender.hpp"

Physics2D::Physics2D()
{
}

Physics2D::~Physics2D()
{
}

void Physics2D::Startup()
{
	m_clock = new Clock();
	m_stepTimer = new Timer();
	m_stepTimer->SetSeconds( m_clock, m_fixedDeltaTime );
	int matrixNum = PHYSICS_LAYER_NUM * PHYSICS_LAYER_NUM;
	for ( int i = 0; i < matrixNum; ++i )
	{
		m_physicsCollisionMatrix[i] = true;
	}
}

void Physics2D::BeginFrame()
{
}

void Physics2D::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	while( m_stepTimer->CheckAndReset() )
	{
		OnFixedUpdate( deltaSeconds );
		SimulateStep( m_fixedDeltaTime );
	}
}

void Physics2D::EndFrame()
{
}

void Physics2D::CleanUpDestroyedObjects()
{
	for( int rigidbodyIndex = 0; rigidbodyIndex < (int)m_rigidbodyList.size(); rigidbodyIndex++ )
	{
		Rigidbody2D* rigidbody2D = m_rigidbodyList[rigidbodyIndex];
		if( rigidbody2D && rigidbody2D->m_readyForDelete )
		{
			delete rigidbody2D;
			m_rigidbodyList[rigidbodyIndex] = nullptr;
		}
	}

	for( int colliderIndex = 0; colliderIndex < (int)m_colliderList.size(); colliderIndex++ )
	{
		Collider2D* collider2D = m_colliderList[colliderIndex];
		if( collider2D && collider2D->m_readyForDelete )
		{
			delete collider2D;
			m_colliderList[colliderIndex] = nullptr;
		}
	}
}

void Physics2D::SimulateStep( float deltaSeconds )
{
	ApplyEffectors( deltaSeconds );	// apply gravity to all dynamic objects
	MoveRigidbodies( deltaSeconds );// apply an euler step to all rigidbodies, and reset per-frame data
	DetectCollisions();	 // determine all pairs of intersecting colliders
	ResolveCollisions(); // resolve all collisions, firing appropraite events
	ApplyObjectsForce( deltaSeconds );
	CleanUpDestroyedObjects();
}

void Physics2D::ApplyEffectors( float deltaSeconds )
{
	Vec2 acceleration = Vec2( 0.f, -m_gravityAmount ) * deltaSeconds;
	for( int rigidbodyIndex = 0; rigidbodyIndex < (int)m_rigidbodyList.size(); rigidbodyIndex++ )
	{
		Rigidbody2D* rigidbody2D = m_rigidbodyList[rigidbodyIndex];
		if ( rigidbody2D )
		{
			rigidbody2D->m_frameForce = Vec2::ZERO;
			if ( rigidbody2D->IsEnablePhysics() )
			{
				if( RIGIDBODY_DYNAMIC_MODE == rigidbody2D->GetSimulationMode() )
				{
					rigidbody2D->SetVelocity( rigidbody2D->GetVelocity() + acceleration );
					rigidbody2D->ApplyDragForce();
				}
			}
		}
	}
}

void Physics2D::MoveRigidbodies( float deltaSeconds )
{
	for( int rigidbodyIndex = 0; rigidbodyIndex < (int)m_rigidbodyList.size(); rigidbodyIndex++ )
	{
		Rigidbody2D* rigidbody2D = m_rigidbodyList[rigidbodyIndex];
		if( rigidbody2D && rigidbody2D->IsEnablePhysics() )
		{
			rigidbody2D->m_frameStartPosition = rigidbody2D->m_worldPosition;

			rigidbody2D->m_worldPosition += rigidbody2D->GetVelocity() * deltaSeconds;
			float angularAcceleration = 0.f;
			if ( rigidbody2D->m_frameTorque != 0.f && rigidbody2D->m_moment != 0.f )
			{
				angularAcceleration = rigidbody2D->m_frameTorque / rigidbody2D->m_moment;
			}
			rigidbody2D->m_angularVelocity += angularAcceleration * deltaSeconds;
			rigidbody2D->m_rotationInRadians += rigidbody2D->m_angularVelocity * deltaSeconds;

			if ( rigidbody2D->m_collider )
			{
				rigidbody2D->GetCollider()->UpdateWorldShape();
			}
		}
	}
}

void Physics2D::DetectCollisions()
{
	for( int objectIndex = 0; objectIndex < (int) m_colliderList.size(); objectIndex++ )
	{
		for( int OtherObjectIndex = 0; OtherObjectIndex < (int) m_colliderList.size(); OtherObjectIndex++ )
		{
			Collider2D* colA = m_colliderList[objectIndex];
			Collider2D* colB = m_colliderList[OtherObjectIndex];
			if ( colA && colB && colA != colB && colA->m_rigidbody->IsEnablePhysics() && colB->m_rigidbody->IsEnablePhysics() )
			{
				bool isInList = false;
				for ( int i = 0; i < (int) m_frameCollisions.size(); i++ )
				{
					if ( (m_frameCollisions[i].me == colA && m_frameCollisions[i].them == colB) || (m_frameCollisions[i].me == colB && m_frameCollisions[i].them == colA) )
					{
						isInList = true;
						break;
					}
				}

				if( !isInList && colA->Intersects( colB ) && 
					HasCollisionBetweenLayers( colA->m_rigidbody->GetPhysicsLayer(), colB->m_rigidbody->GetPhysicsLayer() ) )
				{
					// Only process collisions if the two objects are allowed to interact
					// Only process triggers if the two objects are on the same layer
					bool hasTrigger = colA->m_rigidbody->IsTrigger() || colB->m_rigidbody->IsTrigger();

					if( !hasTrigger || (hasTrigger && colA->m_rigidbody->GetPhysicsLayer() == colB->m_rigidbody->GetPhysicsLayer()) )
					{
						Collision2D collision;
						collision.me = colA;
						collision.them = colB;
						collision.manifold = colA->GetManifold( colB );

						Collision2D inverseCol = collision.GetInverse();
						if( IsNewCollision( collision ) )
						{
							if( collision.me->m_rigidbody->IsTrigger() )
							{
								collision.me->m_rigidbody->OnTriggerEnter( collision );
							}
							if( collision.them->m_rigidbody->IsTrigger() )
							{
								collision.them->m_rigidbody->OnTriggerEnter( inverseCol );
							}
							if( !hasTrigger )
							{
								collision.me->m_rigidbody->OnOverlapEnter( collision );
								collision.them->m_rigidbody->OnOverlapEnter( inverseCol );
							}
						}
						else
						{
							if( collision.me->m_rigidbody->IsTrigger() )
							{
								collision.me->m_rigidbody->OnTriggerStay( collision );
							}
							if( collision.them->m_rigidbody->IsTrigger() )
							{
								collision.them->m_rigidbody->OnTriggerStay( inverseCol );
							}
							if( !hasTrigger )
							{
								collision.me->m_rigidbody->OnOverlapStay( collision );
								collision.them->m_rigidbody->OnOverlapStay( inverseCol );
							}
						}

						m_frameCollisions.push_back( collision );
					}
				}
			}
		}
	}
}

void Physics2D::ResolveCollisions()
{
	CheckLastFrameCollisionExit();
	for ( int collisionIdx = 0; collisionIdx < (int) m_frameCollisions.size(); collisionIdx++ )
	{
		ResolveCollision( m_frameCollisions[collisionIdx] );
		m_lastFrameCollisions.push_back( m_frameCollisions[collisionIdx] );
	}
	m_frameCollisions.clear();
}

void Physics2D::ResolveCollision( Collision2D const&  col )
{
	if ( col.me->m_rigidbody->IsTrigger() || col.them->m_rigidbody->IsTrigger() )
	{
		return;
	}

	// Correcting - stop overlapping
	float myMass = col.me->GetMass();
	float theirMass = col.them->GetMass();
	float pushMe = theirMass / (myMass + theirMass);
	float pushThem = 1.0f - pushMe;

	const eSimulationMode& meRigidbodyType = col.me->m_rigidbody->GetSimulationMode();
	const eSimulationMode& themRigidbodyType = col.them->m_rigidbody->GetSimulationMode();

	// Appply Impulses
	Vec2 r_ap = col.manifold.GetContactPoint() - col.me->m_rigidbody->GetCollider()->GetCenterPoint();
	Vec2 r_bp = col.manifold.GetContactPoint() - col.them->m_rigidbody->GetCollider()->GetCenterPoint();
	float a_normal = 0.f;
	float b_normal = 0.f;
	float a_tangent = 0.f;
	float b_tangent = 0.f;
	float massRatioMe = 0.f;
	float massRatioThem = 0.f;
	Vec2 tangentNormal = col.GetNormal();
	tangentNormal.Rotate90Degrees();

	if ( meRigidbodyType == RIGIDBODY_DYNAMIC_MODE )
	{
		a_normal = (DotProduct2D( r_ap.GetRotated90Degrees(), col.GetNormal() ) * DotProduct2D( r_ap.GetRotated90Degrees(), col.GetNormal() )) / col.me->m_rigidbody->m_moment;
		a_tangent = (DotProduct2D( r_ap.GetRotated90Degrees(), tangentNormal ) * DotProduct2D( r_ap.GetRotated90Degrees(), tangentNormal )) / col.me->m_rigidbody->m_moment;
		massRatioMe = 1 / col.me->m_mass;
	}
	if( themRigidbodyType == RIGIDBODY_DYNAMIC_MODE )
	{
		b_normal = (DotProduct2D( r_bp.GetRotated90Degrees(), col.GetNormal() ) * DotProduct2D( r_bp.GetRotated90Degrees(), col.GetNormal() )) / col.them->m_rigidbody->m_moment;
		b_tangent = (DotProduct2D( r_bp.GetRotated90Degrees(), tangentNormal ) * DotProduct2D( r_bp.GetRotated90Degrees(), tangentNormal )) / col.them->m_rigidbody->m_moment;
		massRatioThem = 1 / col.them->m_mass;
	}

	float bounce = col.me->GetBounceWith( col.them );
	Vec2 myVelocity = col.me->m_rigidbody->GetImpactVelocityAtPoint( col.manifold.GetContactPoint() );
	Vec2 themVelocity = col.them->m_rigidbody->GetImpactVelocityAtPoint( col.manifold.GetContactPoint() );
	float jn = -(1 + bounce) * DotProduct2D( (themVelocity - myVelocity), col.GetNormal() );
	jn /= ((massRatioMe + massRatioThem) + a_normal + b_normal);
	
	float jt = -(1 + bounce) * DotProduct2D( (themVelocity - myVelocity), tangentNormal );
	jt /= ((massRatioMe + massRatioThem) + a_tangent + b_tangent);
	float friction = col.me->GetFrictionWith( col.them );
	if( abs( jt ) > friction * jn )
	{
		jt = -SignFloat( jt ) * jn * friction;
	}

	Vec2 impluse = jn * col.GetNormal() + jt * tangentNormal;
	if( meRigidbodyType == RIGIDBODY_DYNAMIC_MODE )
	{
		col.me->m_rigidbody->ApplyImpulseAt( col.manifold.GetContactPoint(), -impluse );
	}
	if( themRigidbodyType == RIGIDBODY_DYNAMIC_MODE )
	{
		col.them->m_rigidbody->ApplyImpulseAt( col.manifold.GetContactPoint(), impluse );
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Dynamic vs Dynamic (push each other)
	if ( meRigidbodyType == RIGIDBODY_DYNAMIC_MODE && themRigidbodyType == RIGIDBODY_DYNAMIC_MODE )
	{
		col.me->m_rigidbody->m_worldPosition += pushMe * col.GetNormal() * col.GetPenetration();
		col.them->m_rigidbody->m_worldPosition += -pushThem * col.GetNormal() * col.GetPenetration();
	}
	// Kinematic vs Kinematic (push each other)
	else if( meRigidbodyType == RIGIDBODY_KINEMATIC_MODE && themRigidbodyType == RIGIDBODY_KINEMATIC_MODE )
	{
		col.me->m_rigidbody->m_worldPosition += pushMe * col.GetNormal() * col.GetPenetration();
		col.them->m_rigidbody->m_worldPosition += -pushThem * col.GetNormal() * col.GetPenetration();
	}
	// Dynamic hits a Kinematic (Only Dynamic should push)
	// Dynamics vs (Kinematic || Static) -> Only push dyanmic 100%
	if( meRigidbodyType == RIGIDBODY_DYNAMIC_MODE && (themRigidbodyType == RIGIDBODY_KINEMATIC_MODE || themRigidbodyType == RIGIDBODY_STATIC_MODE) )
	{
		col.me->m_rigidbody->m_worldPosition += col.GetNormal() * col.GetPenetration();
	}
	if( themRigidbodyType == RIGIDBODY_DYNAMIC_MODE && (meRigidbodyType == RIGIDBODY_KINEMATIC_MODE || meRigidbodyType == RIGIDBODY_STATIC_MODE) )
	{
		col.them->m_rigidbody->m_worldPosition += -col.GetNormal() * col.GetPenetration();
	}
	// Kinematic vs Static -> Only push kinematic 100%
	if( meRigidbodyType == RIGIDBODY_KINEMATIC_MODE && themRigidbodyType == RIGIDBODY_STATIC_MODE )
	{
		col.me->m_rigidbody->m_worldPosition += col.GetNormal() * col.GetPenetration();
	}
	if( themRigidbodyType == RIGIDBODY_KINEMATIC_MODE && meRigidbodyType == RIGIDBODY_STATIC_MODE )
	{
		col.them->m_rigidbody->m_worldPosition += -col.GetNormal() * col.GetPenetration();
	}
	// STATICs don't move
}

void Physics2D::ApplyObjectsForce( float deltaSeconds )
{
	for( int rigidbodyIndex = 0; rigidbodyIndex < (int)m_rigidbodyList.size(); rigidbodyIndex++ )
	{
		Rigidbody2D* rigidbody2D = m_rigidbodyList[rigidbodyIndex];
		if( rigidbody2D && rigidbody2D->m_collider )
		{
			rigidbody2D->m_velocity += (rigidbody2D->m_frameForce / rigidbody2D->m_collider->GetMass()) * deltaSeconds;
		}
	}
}

void Physics2D::CheckLastFrameCollisionExit()
{
	for( Collision2D lastFrameCol : m_lastFrameCollisions )
	{
		bool isExistInThisFrame = false;
		for( Collision2D thisFrameCol : m_frameCollisions )
		{
			if( lastFrameCol.me == thisFrameCol.me && lastFrameCol.them == thisFrameCol.them )
			{
				isExistInThisFrame = true;
				break;
			}
		}
		if ( !isExistInThisFrame )
		{
			bool hasTrigger = false;
			Collision2D inverseCol = lastFrameCol.GetInverse();
			if ( lastFrameCol.me->m_rigidbody->IsTrigger() )
			{
				lastFrameCol.me->m_rigidbody->OnTriggerExit( lastFrameCol );
				hasTrigger = true;
			}
			if ( lastFrameCol.them->m_rigidbody->IsTrigger() )
			{
				lastFrameCol.them->m_rigidbody->OnTriggerExit( inverseCol );
				hasTrigger = true;
			}
			if ( !hasTrigger )
			{
				lastFrameCol.me->m_rigidbody->OnOverlapExit( lastFrameCol );
				lastFrameCol.them->m_rigidbody->OnOverlapExit( inverseCol );
			}
		}
	}
	m_lastFrameCollisions.clear();
}

bool Physics2D::IsNewCollision( const Collision2D& col )
{
	for ( Collision2D lastFrameCol : m_lastFrameCollisions )
	{
		if ( lastFrameCol.me == col.me && lastFrameCol.them == col.them )
		{
			return false;
		}
	}
	return true;
}

Rigidbody2D* Physics2D::CreateRigidbody()
{
	Rigidbody2D* rb = new Rigidbody2D();
	rb->m_system = this;
	m_rigidbodyList.push_back(rb);
	return rb;
}

void Physics2D::DestroyRigidbody( Rigidbody2D* rb )
{
	Collider2D* collider = rb->GetCollider();
	if( collider != nullptr ) {
		rb->TakeCollider( nullptr );
		collider->Destroy();
	}
}

DiscCollider2D* Physics2D::CreateDiscCollider( Vec2 localPosition, float radius )
{
	DiscCollider2D* discCollider = new DiscCollider2D();
	discCollider->m_radius = radius;
	discCollider->m_localPosition = localPosition;
	discCollider->m_type = COLLIDER2D_DISC;
	discCollider->m_system = this;

	m_colliderList.push_back(discCollider);
	return discCollider;
}

PolygonCollider2D* Physics2D::CreatePolygonCollider( Vec2 const* points, uint pointCount, bool isMakeConvexFromPointCloud )
{
	PolygonCollider2D* polygonCollider = new PolygonCollider2D();
	if ( isMakeConvexFromPointCloud )
	{
		polygonCollider->m_polygon2 = Polygon2::MakeConvexFromPointCloud( points, pointCount );
	}
	else
	{
		polygonCollider->m_polygon2 = Polygon2::MakeFromLineLoop( points, pointCount );
	}
	polygonCollider->m_type = COLLIDER2D_POLYGON;
	polygonCollider->m_system = this;

	m_colliderList.push_back(polygonCollider);
	return polygonCollider;
}

void Physics2D::DestroyCollider( Collider2D* collider )
{
	collider->Destroy();
}

bool Physics2D::HasCollisionBetweenLayers( ePhysicsLayer layerA, ePhysicsLayer layerB )
{
	return m_physicsCollisionMatrix[ layerA * PHYSICS_LAYER_NUM + layerB ];
}

void Physics2D::SetSceneGravity( float gravityAmount )
{
	m_gravityAmount = gravityAmount;
}

void Physics2D::SetFixedDeltaTime( float frameTimeSeconds )
{
	m_fixedDeltaTime = 1.f / frameTimeSeconds;
	m_stepTimer->SetSeconds( m_clock, m_fixedDeltaTime );
}

void Physics2D::SetPhysicsLayer( ePhysicsLayer layerA, ePhysicsLayer layerB, bool isCollision )
{
	uint indexA = layerA * PHYSICS_LAYER_NUM + layerB;
	uint indexB = layerB * PHYSICS_LAYER_NUM + layerA;
	m_physicsCollisionMatrix[indexA] = isCollision;
	m_physicsCollisionMatrix[indexB] = isCollision;
}
