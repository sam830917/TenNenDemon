#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/MathUtils.hpp"

PolygonCollider2D::PolygonCollider2D()
{
}

PolygonCollider2D::~PolygonCollider2D()
{
}

void PolygonCollider2D::UpdateWorldShape()
{
	if( nullptr != m_rigidbody )
	{
		m_worldPosition = m_rigidbody->m_worldPosition;
	}
	// set world bound
	Polygon2 polygon = m_polygon2;
	polygon.RotateInRadian( m_rigidbody->GetRotationInRadian() );
	std::vector<Vec2> points = polygon.GetPoints();
	Vec2 mostBottomVec = points[0];
	Vec2 mostTopVec = points[0];
	Vec2 mostLeftVec = points[0];
	Vec2 mostRightVec = points[0];
	for( int index = 1; index < (int)points.size(); index++ )
	{
		Vec2& point = points[index];

		if( mostBottomVec.y > point.y )
		{
			mostBottomVec = point;
		}
		if( mostTopVec.y < point.y )
		{
			mostTopVec = point;
		}
		if( mostLeftVec.x > point.x )
		{
			mostLeftVec = point;
		}
		if( mostRightVec.x < point.x )
		{
			mostRightVec = point;
		}
	}
	AABB2 bound = AABB2( Vec2( mostLeftVec.x, mostBottomVec.y ), Vec2( mostRightVec.x, mostTopVec.y ) );
	bound.Translate( m_worldPosition );
	m_worldBound = bound;
}

Vec2 PolygonCollider2D::GetClosestPoint( Vec2 pos ) const
{
	return GetNearestPointOnPolygon2D( pos, GetWorldPositionPolygon() );
}

Vec2 PolygonCollider2D::GetCenterPoint() const
{
	return m_polygon2.GetCenterPoint() + m_worldPosition;
}

bool PolygonCollider2D::Contains( Vec2 pos ) const
{
	return IsPointInsidePolygon2D( pos, GetWorldPositionPolygon() );
}

void PolygonCollider2D::DebugRender( RenderContext* ctx, Rgba8 const& borderColor, Rgba8 const& fillColor )
{
	Polygon2 polygon = m_polygon2;
	polygon.RotateInRadian( m_rigidbody->GetRotationInRadian() );
	polygon.Translate( m_worldPosition );

	std::vector<Vec2> polygonPoints = polygon.GetPoints();
	ctx->DrawLinesFromPoints( (int)polygonPoints.size(), &polygonPoints[0], borderColor, 0.1f );
	ctx->DrawPolyGon2D( polygon, fillColor );

	ctx->DrawCircle( GetCenterPoint(), 0.1f, Rgba8::RED, 0.f );
	//ctx->DrawCircle( GetCenterPoint(), GetCosmeticRadius(), Rgba8(255,255,255,100), 0.f );
}

void PolygonCollider2D::Destroy()
{
	m_readyForDelete = true;
}

Vec2 PolygonCollider2D::GetBottomPosition()
{
	Polygon2 polygon = m_polygon2;
	polygon.RotateInRadian( m_rigidbody->GetRotationInRadian() );
	std::vector<Vec2> points = polygon.GetPoints();
	Vec2 mostBottomVec = points[0];
	for( int index = 1; index < (int) points.size(); index++ )
	{
		Vec2& point = points[index];
		if ( mostBottomVec.y > point.y )
		{
			mostBottomVec = point;
		}
	}
	return mostBottomVec + m_worldPosition;
}

float PolygonCollider2D::GetCosmeticRadius()
{
	Vec2 centerPos = GetCenterPoint();
	Polygon2 polygon = m_polygon2;
	polygon.RotateInRadian( m_rigidbody->GetRotationInRadian() );
	std::vector<Vec2> points = polygon.GetPoints();
	float farthestDistance = GetDistance2D( centerPos, points[0] + m_worldPosition );
	for( int index = 1; index < (int)points.size(); index++ )
	{
		float pointDistance = GetDistance2D( centerPos, points[index] + m_worldPosition );
		if ( pointDistance > farthestDistance )
		{
			farthestDistance = pointDistance;
		}
	}

	return farthestDistance;
}

AABB2 PolygonCollider2D::GetWorldBounds()
{
	return m_worldBound;
}

float PolygonCollider2D::CalculateMoment( float mass )
{
	std::vector<Vec2> points = m_polygon2.GetPoints();
	Vec2 centerPoint = m_polygon2.GetCenterPoint();
	int length = (int) points.size();
	float sum1 = 0.f;
	float sum2 = 0.f;
	for( int i = 0; i < length; i++ )
	{
		Vec2 point1 = points[i] - centerPoint;
		Vec2 point2 = points[(i + 1) % length] - centerPoint;
		float cross = CrossProduct2D( point1, point2 );
		sum1 += cross * (DotProduct2D( point1, point1 ) + DotProduct2D( point1, point2 ) + DotProduct2D( point2, point2 ));
		sum2 += cross;
	}
	return mass / 6 * sum1 / sum2;
}

Polygon2 PolygonCollider2D::GetWorldPositionPolygon() const
{
	Polygon2 polygon = m_polygon2;
	polygon.RotateInRadian( m_rigidbody->GetRotationInRadian() );
	polygon.Translate( m_worldPosition );

	return polygon;
}

Vec2 PolygonCollider2D::Support( const Vec2& direction ) const
{	
	return GetWorldPositionPolygon().Support( direction );
}
