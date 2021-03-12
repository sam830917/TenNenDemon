#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

void Rigidbody2D::Destroy()
{
	m_readyForDelete = true;
	m_system->DestroyRigidbody( this );
	m_enablePhysics = false;
}

void Rigidbody2D::TakeCollider( Collider2D* collider )
{
	// destroying my current one if present
	if (m_collider)
	{
		m_system->DestroyCollider( m_collider );
		m_collider = nullptr;
	}
	m_collider = collider;

	if (collider)
	{
		collider->m_rigidbody = this;
		m_collider->UpdateWorldShape();
		m_moment = m_collider->CalculateMoment( collider->GetMass() );
	}
}

void Rigidbody2D::SetPosition( Vec2 position )
{
	m_worldPosition = position;
	if( m_collider )
	{
		m_collider->UpdateWorldShape();
	}
}

void Rigidbody2D::Translate( Vec2 translation )
{
	if( m_collider )
	{
		SetPosition( m_worldPosition + translation );
	}
}

void Rigidbody2D::ApplyImpulseAt( Vec2 worldPos, Vec2 impulse )
{
	// apply linear impulse
	m_velocity += (impulse / m_collider->GetMass());
	// apply angular impulse
	Vec2 localImpact = worldPos - GetCollider()->GetCenterPoint();
	Vec2 directionOfTorque = localImpact.GetRotated90Degrees();
	float impulseTorque = DotProduct2D( impulse, directionOfTorque );
	m_angularVelocity += impulseTorque / m_moment;
}

void Rigidbody2D::AddForce( Vec2 force )
{
	m_frameForce += force;
}

void Rigidbody2D::ApplyDragForce()
{
	Vec2 velocity = GetVerletVelocity();
	Vec2 dragForce = -velocity * m_drag;
	AddForce( dragForce );
}

void Rigidbody2D::AddDrag( float amount )
{
	m_drag += amount;
	if ( m_drag < 0.f )
	{
		m_drag = 0.f;
	}
}

Vec2 Rigidbody2D::GetVerletVelocity()
{
	return (m_worldPosition - m_frameStartPosition) / m_system->m_fixedDeltaTime;
}

Vec2 Rigidbody2D::GetImpactVelocityAtPoint( Vec2 worldPos )
{
	Vec2 displacement = worldPos - GetCollider()->GetCenterPoint();
	return GetVelocity() + displacement.GetRotated90Degrees() * m_angularVelocity;
}

void Rigidbody2D::SetVelocity( const Vec2& velocity )
{
	m_velocity = velocity;
}

void Rigidbody2D::SetUserData( uint type, void* data )
{
	m_userDataType = type;
	m_userData = data;
}

Rigidbody2D::~Rigidbody2D()
{
	GUARANTEE_OR_DIE( m_collider == nullptr, "Collider is not deleted on Rigibody2D" );
}