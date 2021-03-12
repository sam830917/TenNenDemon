#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Math/MathUtils.hpp"

DiscCollider2D::DiscCollider2D()
{
}

DiscCollider2D::~DiscCollider2D()
{
}

void DiscCollider2D::UpdateWorldShape()
{
	if( nullptr != m_rigidbody )
	{
		m_worldPosition = m_rigidbody->m_worldPosition;
	}
	else
	{
		m_worldPosition = m_localPosition;
	}
	AABB2 bound = AABB2( Vec2( -m_radius, -m_radius ), Vec2( m_radius, m_radius ) );
	bound.Translate( m_worldPosition + m_localPosition );
	m_worldBound = bound;
}

Vec2 DiscCollider2D::GetClosestPoint( Vec2 pos ) const
{
	return GetNearestPointOnDisc2D( pos, m_worldPosition, m_radius );
}

Vec2 DiscCollider2D::GetCenterPoint() const
{
	return m_worldPosition;
}

bool DiscCollider2D::Contains( Vec2 pos ) const
{
	return IsPointInsideDisc2D( pos, m_worldPosition, m_radius );
}

void DiscCollider2D::DebugRender( RenderContext* ctx, Rgba8 const& borderColor, Rgba8 const& fillColor )
{
	ctx->DrawCircle( m_worldPosition, m_radius, fillColor, 0.f );
	Vec2 radiusRenderLine = Vec2( m_radius, 0.f );
	radiusRenderLine.RotateRadians( m_rigidbody->GetRotationInRadian() );
	radiusRenderLine += m_worldPosition;
	ctx->DrawLine( m_worldPosition, radiusRenderLine , Rgba8::RED, 0.1f );
	ctx->DrawRing( m_worldPosition, m_radius, borderColor, 0.1f );
	ctx->DrawCircle( m_worldPosition, 0.1f, Rgba8::BLUE, 0.f );
}

void DiscCollider2D::Destroy()
{
	m_readyForDelete = true;
}

Vec2 DiscCollider2D::GetBottomPosition()
{
	return m_worldPosition + m_localPosition + Vec2( 0.f, -1.f ) * m_radius;
}

float DiscCollider2D::GetCosmeticRadius()
{
	return m_radius;
}

AABB2 DiscCollider2D::GetWorldBounds()
{
	return m_worldBound;
}

float DiscCollider2D::CalculateMoment( float mass )
{
	return (mass * m_radius * m_radius) * 0.5f;
}
