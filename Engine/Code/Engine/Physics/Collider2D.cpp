#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Math/Plane2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/DebugRender.hpp"

typedef bool (*collision_check_cb)(Collider2D const*, Collider2D const*);
typedef Manifold2 (*collision_manifold)(Collider2D const*, Collider2D const*);

void PhysicsMaterial::AddRestitution( float amount )
{
	restitution += amount;
	restitution = Clamp( restitution, 0.f, 1.f );
}

void PhysicsMaterial::AddFriction( float amount )
{
	friction += amount;
	friction = Clamp( friction, 0.f, 1.f );
}

Collider2D::~Collider2D()
{
}

static bool DiscVDiscCollisionCheck( Collider2D const* col0, Collider2D const* col1 )
{
	// this function is only called if the types tell me these casts are safe - so no need to a dynamic cast or type checks here.
	DiscCollider2D const* disc0 = (DiscCollider2D const*)col0;
	DiscCollider2D const* disc1 = (DiscCollider2D const*)col1;

	return DoDiscsOverlap( disc0->m_worldPosition, disc0->m_radius, disc1->m_worldPosition, disc1->m_radius );
}

static bool DiscVPolygonCollisionCheck( Collider2D const* col0, Collider2D const* col1 )
{
	DiscCollider2D const* disc = (DiscCollider2D const*)col0;
	PolygonCollider2D const* polygon = (PolygonCollider2D const*)col1;

	Vec2 nearestPoint = polygon->GetClosestPoint( disc->m_worldPosition );
	if( disc->m_radius >= GetDistance2D( nearestPoint, disc->m_worldPosition ) )
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------
struct Edge
{
	float distance;
	Vec2 normal;
	int index;

};

Edge FindClosestEdge( bool isClockwise, std::vector<Vec2>& vertices )
{
	float closestDistance = 0.f;
	Vec2 closestNormal = Vec2();
	int closestIndex = 0;

	for ( int i = 0; i < (int) vertices.size(); ++i )
	{
		int j = i + 1;
		if ( j >= (int) vertices.size() )
		{
			j = 0;
		}

		Vec2 line = vertices[i] - vertices[j];
		Vec2 norm;
		isClockwise ? norm = Vec2( line.y, -line.x ) : norm = Vec2( -line.y, line.x );
		norm.Normalize();
		float dist = DotProduct2D( vertices[i], norm );
		if ( dist < closestDistance || i == 0 )
		{
			closestDistance = dist;
			closestNormal = norm;
			closestIndex = j;
		}
	}
	Edge edge;
	edge.distance = closestDistance;
	edge.normal = closestNormal;
	edge.index = closestIndex;

	return edge;
}

bool AddSupport( std::vector<Vec2>& vertices, const Vec2& direction, const PolygonCollider2D& polygon0, const PolygonCollider2D& polygon1 )
{
	Vec2 newVertex = polygon0.Support( direction ) - polygon1.Support( -direction );
	vertices.push_back( newVertex );
	float length = DotProduct2D( direction, newVertex );
	return length > 0.f;
}

Vec2 TripleProduct( const Vec2& a, const Vec2& b, const Vec2& c )
{
	Vec3 A = Vec3( a.x, a.y, 0.f );
	Vec3 B = Vec3( b.x, b.y, 0.f );
	Vec3 C = Vec3( c.x, c.y, 0.f );

	Vec3 first = CrossProduct3D( A, B );
	Vec3 second = CrossProduct3D( first, C );

	return Vec2( second.x, second.y );
}
static std::vector<Vec2> sPvsPVertices;

static bool PolygonVPolygonCollisionCheck( Collider2D const* col0, Collider2D const* col1 )
{
	const PolygonCollider2D* polygon0 = (PolygonCollider2D const*)col0;
	const PolygonCollider2D* polygon1 = (PolygonCollider2D const*)col1;

	std::vector<Vec2> vertices;
	Vec2 direction = Vec2::ZERO;
	bool result = true;
	while ( result )
	{
		switch( (int)vertices.size() )
		{
		case 0:
		{
			direction = polygon1->GetCenterPoint() - polygon0->GetCenterPoint();
			break;
		}
		case 1:
		{
			direction *= -1.f;
			break;
		}
		case 2:
		{
			Vec2 b = vertices[1];
			Vec2 c = vertices[0];
			Vec2 cb = b - c;
			Vec2 c0 = c * -1;

			direction = TripleProduct( cb, c0, cb );
			break;
		}
		case 3:
		{
			Vec2 a  = vertices[2];
			Vec2 b  = vertices[1];
			Vec2 c  = vertices[0];
			Vec2 a0  = -a;		// v2 to the origin
			Vec2 ab  = b - a;	// v2 to v1
			Vec2 ac  = c - a;	// v2 to v0

			Vec2 abPerp  = TripleProduct( ac, ab, ab );
			Vec2 acPerp  = TripleProduct( ab, ac, ac );

			if( DotProduct2D( abPerp, a0 ) > 0.f )
			{
				vertices.erase( vertices.begin() );
				direction = abPerp;
			}
			else if( DotProduct2D( acPerp, a0 ) > 0.f )
			{
				vertices.erase( vertices.begin() + 1 );
				direction = acPerp;
			}
			else
			{
				sPvsPVertices = vertices;
				return true;
			}
			break;
		}
		}
		result = AddSupport( vertices, direction, *polygon0, *polygon1 );
	}

	return false;
}
//--------------------------------------------------------------------------------------------------------------------------------------

static bool PolygonVDiscCollisionCheck( Collider2D const* col0, Collider2D const* col1 )
{
	return DiscVPolygonCollisionCheck( col1, col0 );
}

// a "matrix" lookup is just a 2D array
static collision_check_cb gCollisionChecks[NUM_COLLIDER_TYPES * NUM_COLLIDER_TYPES] =
{
	/*             disc,						polygon, */
	/*    disc */  DiscVDiscCollisionCheck,     PolygonVDiscCollisionCheck,
	/* polygon */  DiscVPolygonCollisionCheck,  PolygonVPolygonCollisionCheck
};

static Manifold2 GetDiscVDiscCollisionManifold( Collider2D const* col0, Collider2D const* col1 )
{
	// this function is only called if the types tell me these casts are safe - so no need to a dynamic cast or type checks here.
	DiscCollider2D const* disc0 = (DiscCollider2D const*)col0;
	DiscCollider2D const* disc1 = (DiscCollider2D const*)col1;

	Manifold2 manifold;
	Vec2 displacement = disc0->m_worldPosition - disc1->m_worldPosition;
	displacement.Normalize();

	float penetration = 0.f;
	float distance = GetDistance2D( disc1->m_worldPosition, disc0->m_worldPosition );
	if ( IsPointInsideDisc2D( disc1->m_worldPosition, disc0->m_worldPosition, disc0->m_radius ) )
	{
		if ( disc0->m_radius == distance )
		{
			penetration = disc1->m_radius;
		}
		else
		{
			penetration = disc0->m_radius - distance + disc1->m_radius;
		}
	}
	else
	{
		penetration = (disc0->m_radius + disc1->m_radius) - GetDistance2D( disc1->m_worldPosition, disc0->m_worldPosition );
	}

	manifold.normal = displacement;
	manifold.penetration = penetration;
	manifold.SetContactPoint(disc0->m_worldPosition - displacement * (disc0->m_radius - penetration * 0.5f));
	return manifold;
}
static Manifold2 GetDiscVPolygonCollisionManifold( Collider2D const* col0, Collider2D const* col1 )
{
	DiscCollider2D const* disc = (DiscCollider2D const*)col0;
	PolygonCollider2D const* polygon = (PolygonCollider2D const*)col1;

	Manifold2 manifold;
	Vec2 nearestPoint = polygon->GetWorldPositionPolygon().GetPointOnNearestEdge( disc->m_worldPosition );
	Vec2 displacement = Vec2::ZERO;
	// rare condition
	if ( disc->m_worldPosition == nearestPoint )
	{
		nearestPoint = nearestPoint + Vec2( 0.001f, 0.001f );
	}
	displacement = disc->m_worldPosition - nearestPoint;
	displacement.Normalize();

	float penetration = 0.f;
	if ( IsPointInsidePolygon2D( disc->m_worldPosition, polygon->GetWorldPositionPolygon() ) )
	{
		displacement = -displacement;
		penetration = disc->m_radius + GetDistance2D( disc->m_worldPosition, nearestPoint );
	}
	else
	{
		float distance = GetDistance2D( disc->m_worldPosition, nearestPoint );
		penetration = disc->m_radius - distance;
	}

	manifold.normal = displacement;
	manifold.penetration = penetration;
	manifold.SetContactPoint( disc->m_worldPosition - displacement * (disc->m_radius - penetration * 0.5f) );
	return manifold;
}

static Manifold2 GetPolygonVPolygonCollisionManifold( Collider2D const* col0, Collider2D const* col1 )
{
	const PolygonCollider2D* polygon0 = (PolygonCollider2D const*)col0;
	const PolygonCollider2D* polygon1 = (PolygonCollider2D const*)col1;

	std::vector<Vec2> points;
	for( int i = 0; i < polygon0->m_polygon2.GetVertexCount(); ++i )
	{
		for( int j = 0; j < polygon1->m_polygon2.GetVertexCount(); ++j )
		{
			Vec2 pos0 = polygon0->GetWorldPositionPolygon().GetPoints()[i];
			Vec2 pos1 = polygon1->GetWorldPositionPolygon().GetPoints()[j];
			points.push_back( pos0 - pos1 );
		}
	}
	Polygon2 polygon = Polygon2::MakeConvexFromPointCloud( &points[0], (int)points.size() );


	Manifold2 manifold;
	Edge edge;
	for( int i = 0; i < (int) polygon.GetVertexCount(); ++i )
	{
		Polygon2* pol = new Polygon2();
		pol->AddNewPoints( &sPvsPVertices[0], (int)sPvsPVertices.size() );
		pol->OrderPointPositions();

		std::vector<Vec2> polygonPoints = pol->GetPoints();
		edge = FindClosestEdge( false, polygonPoints );
		Vec2 support = polygon0->Support( edge.normal ) - polygon1->Support( -edge.normal );
		float distance = DotProduct2D( support, edge.normal );

		if( abs( distance - edge.distance ) <= 0.001 ) 
		{
			break;
		}
		else 
		{
			sPvsPVertices.push_back( support );
		}
	}
	manifold.penetration = abs(edge.distance);
	manifold.normal = -edge.normal;

	// Contact point
	Polygon2 worldPoly0 = polygon0->GetWorldPositionPolygon();
	Polygon2 worldPoly1 = polygon1->GetWorldPositionPolygon();
	Vec2 supportPoint = worldPoly1.Support( manifold.normal );
	Plane2 r = Plane2( manifold.normal, supportPoint );

	std::vector<Vec2> planePoints;
	for ( int i = 0; i < worldPoly1.GetVertexCount(); ++i  )
	{
		Vec2 vertex = worldPoly1.GetPoints()[i];
		float distance = abs( r.GetSignedDistanceFromPlane( vertex ) );
		if ( distance <= 0.001f )
		{
			planePoints.push_back(vertex);
		}
	}

	Vec2 t = manifold.normal.GetRotatedMinus90Degrees();
	LineSegment2 referEdge;
	float maxLength = 0.f;
	float minLength = 0.f;
	for ( int i = 0; i < (int) planePoints.size(); ++i )
	{
		Vec2 point = planePoints[i];
		float length = DotProduct2D( t, point );
		if ( length > maxLength || i == 0 )
		{
			maxLength = length;
			referEdge.m_end = planePoints[i];
		}
		if( length < minLength || i == 0 )
		{
			minLength = length;
			referEdge.m_start = planePoints[i];
		}
	}

	std::vector<Vec2> contactPoints;
	if ( referEdge.m_start == referEdge.m_end )
	{
		contactPoints.push_back( referEdge.m_start - manifold.normal * manifold.penetration );
	}
	else
	{
		for( int startIdx = 0; startIdx < worldPoly0.GetVertexCount(); ++startIdx )
		{
			int endIdx = startIdx + 1;
			if( startIdx == worldPoly0.GetVertexCount() - 1 )
			{
				endIdx = 0;
			}

			Vec2 startPos = worldPoly0.GetPoints()[startIdx];
			Vec2 endPos = worldPoly0.GetPoints()[endIdx];
			LineSegment2 edgeLine = LineSegment2( startPos, endPos );
			LineSegment2 clippedLine;
			if( LineSegment2::ClipSegmentToSegment( edgeLine, referEdge, clippedLine ) )
			{
				if( DotProduct2D( clippedLine.m_start - referEdge.m_start, manifold.normal ) <= 0.f )
				{
					contactPoints.push_back( clippedLine.m_start );
				}
				if( DotProduct2D( clippedLine.m_end - referEdge.m_start, manifold.normal ) <= 0.f )
				{
					contactPoints.push_back( clippedLine.m_end );
				}
			}
		}
	}

	Vec2 min;
	float minDistance = 0.f;
	Vec2 max;
	float maxDistance = 0.f;
	for( int i = 0; i < (int)contactPoints.size(); ++i )
	{
		Vec2 contactPos = contactPoints[i];
		float distance = DotProduct2D( contactPos - referEdge.m_start, referEdge.m_end - referEdge.m_start );
		if( distance < minDistance || i == 0 )
		{
			min = contactPos;
			minDistance = distance;
		}
		if( distance > maxDistance || i == 0 )
		{
			max = contactPos;
			maxDistance = distance;
		}
	}

	manifold.SetContactPoint( min, max );

	return manifold;
}

static Manifold2 GetPolygonVDiscCollisionManifold( Collider2D const* col0, Collider2D const* col1 )
{
	Manifold2 manifold = GetDiscVPolygonCollisionManifold( col1, col0 );
	manifold.normal = -manifold.normal;
	return manifold;
}

static collision_manifold gCollisionManifolds[NUM_COLLIDER_TYPES * NUM_COLLIDER_TYPES] =
{
	/*             disc,								polygon, */
	/*    disc */  GetDiscVDiscCollisionManifold,     GetPolygonVDiscCollisionManifold,
	/* polygon */  GetDiscVPolygonCollisionManifold,  GetPolygonVPolygonCollisionManifold
};

bool Collider2D::Intersects( Collider2D const* other ) const
{
	if ( !DoAABB2Overlap( m_worldBound, other->m_worldBound ) )
	{
		return false;
	}
	eCollider2DType myType = m_type;
	eCollider2DType otherType = other->m_type;

	int idx = otherType * NUM_COLLIDER_TYPES + myType;
	collision_check_cb check = gCollisionChecks[idx];
	return check( this, other );
}

Manifold2 Collider2D::GetManifold( Collider2D const* other )
{
	if( !DoAABB2Overlap( m_worldBound, other->m_worldBound ) )
	{
		return Manifold2();
	}
	eCollider2DType myType = m_type;
	eCollider2DType otherType = other->m_type;

	// flip the types when looking into the index.
	int idx = otherType * NUM_COLLIDER_TYPES + myType;
	collision_manifold manifold = gCollisionManifolds[idx];
	return manifold( this, other );
}

float Collider2D::GetBounceWith( Collider2D const* other ) const
{
	return m_material.restitution * other->m_material.restitution;
}

float Collider2D::GetFrictionWith( Collider2D const* other ) const
{
	return m_material.friction * other->m_material.friction;
}

void Collider2D::AddMass( float amount )
{
	m_mass += amount;
	if ( m_mass < 0.001f )
	{
		m_mass = 0.001f;
	}
	m_rigidbody->m_moment = CalculateMoment( m_mass );
}