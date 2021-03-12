#include "Engine/Math/Polygon2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"

Polygon2::~Polygon2()
{
}

bool Polygon2::IsValid() const
{
	return ( (int) m_points.size() >= 3 ) ? true : false;
}

bool Polygon2::IsConvex() const
{
	if ( !IsValid() )
	{
		return false;
	}

	Vec2 rightVertex = m_points[1];
	Vec2 currentVertex = m_points[0];
	Vec2 leftVertex = m_points[GetVertexCount() - 1];
	bool sameDirection = CrossProduct2D( Vec2( currentVertex - leftVertex ), Vec2( rightVertex - currentVertex ) ) < 0;

	for( int index = 1; index < GetVertexCount(); index++ )
	{
		rightVertex = Vec2::ZERO;
		currentVertex = m_points[index];
		leftVertex = Vec2::ZERO;

		if( index == GetVertexCount() - 1 )
		{
			rightVertex = m_points[0];
			leftVertex = m_points[index-1];
		}
		else
		{
			rightVertex = m_points[index+1];
			leftVertex = m_points[index-1];
		}

		if( CrossProduct2D( Vec2( currentVertex - leftVertex ), Vec2( rightVertex - currentVertex ) ) < 0 != sameDirection )
		{
			return false;
		}
	}
	return true;
}

float Polygon2::GetDistance( Vec2 point ) const
{
	Vec2 nearestPoint = GetNearestPointOnPolygon2D( point, *this );
	return ( nearestPoint - point ).GetLength();
}

void Polygon2::Translate( const Vec2& translation )
{
	for( int index = 0; index < (int) m_points.size(); index++ )
	{
		Vec2& point = m_points[index];
		point += translation;
	}
}

Vec2 Polygon2::GetCenterPoint() const
{
	Vec2 pos;
	for( int index = 0; index < (int)m_points.size(); index++ )
	{
		const Vec2& point = m_points[index];
		pos += point;
	}
	return pos / (float)m_points.size();
	//float area = 0.f;
	//Vec2 center = Vec2::ZERO;
	//for( int i = 0; i < (int) m_points.size() - 1; i++ )
	//{
	//	float pointsCrossProduce = CrossProduct2D( m_points[i], m_points[i+1] );
	//	area += pointsCrossProduce * 0.5f;
	//	center += pointsCrossProduce * ( m_points[i] + m_points[i+1] );
	//}
	//float pointsCrossProduce = CrossProduct2D( m_points[(int) m_points.size() - 1], m_points[0] );
	//area += pointsCrossProduce * 0.5f;
	//center += pointsCrossProduce * ( m_points[(int) m_points.size() - 1] + m_points[0] );
	//center /= 6 * area;
	//
	//return center;
}

void Polygon2::OrderPointPositions()
{
	// order by counter-clock wise
	int pointCount = GetVertexCount();
	std::vector<float> pointsAngles;
	std::vector<Vec2> pointsList;
	pointsAngles.reserve( pointCount );
	pointsList.reserve( pointCount );
	// Get all points angles, and reset to local position
	Vec2 centerPoint = GetCenterPoint();
	for( int index = 0; index < pointCount; index++ )
	{
		pointsList.push_back( m_points[index] - centerPoint );
		pointsAngles.push_back( pointsList[index].GetAngleDegrees() );
	}

	for( int firstIdx = 0; firstIdx < pointCount-1; firstIdx++ )
	{
		for( int secondIdx = pointCount-1; secondIdx > firstIdx; secondIdx-- )
		{
			if( pointsAngles[secondIdx] < pointsAngles[secondIdx-1] )
			{
				float temp = pointsAngles[secondIdx];
				pointsAngles[secondIdx] = pointsAngles[secondIdx-1];
				pointsAngles[secondIdx-1] = temp;

				Vec2 tempVec2 = pointsList[secondIdx];
				pointsList[secondIdx] = pointsList[secondIdx-1];
				pointsList[secondIdx-1] = tempVec2;
			}
		}
	}
	m_points = pointsList;
	Translate(centerPoint);
}

void Polygon2::AddNewPoints( Vec2 const* points, uint pointCount )
{
	for( uint index = 0; index < pointCount; index++ )
	{
		Vec2 point = points[index];
		m_points.push_back( point );
	}
}

Vec2 Polygon2::GetPointOnNearestEdge( const Vec2& refPos ) const
{
	std::vector<Vec2> points = GetPoints();
	std::vector<Vec2> edgePoints;
	for( int idx = 0; idx < GetVertexCount(); idx++ )
	{
		if( idx != GetVertexCount() - 1 )
		{
			Vec2 point = GetNearestPointOnLineSegment2D( refPos, points[idx], points[idx+1] );
			edgePoints.push_back( point );
		}
		else
		{
			Vec2 point = GetNearestPointOnLineSegment2D( refPos, points[idx], points[0] );
			edgePoints.push_back( point );
		}
	}

	int shortestIdx = 0;
	for( int idx = 0; idx < (int)edgePoints.size(); idx++ )
	{
		float leng = GetDistance2D( edgePoints[idx], refPos );
		if( leng < GetDistance2D( edgePoints[shortestIdx], refPos ) )
		{
			shortestIdx = idx;
		}
	}

	return edgePoints[shortestIdx];
}

void Polygon2::RotateInRadian( float radian )
{
	Vec2 centerPoint = GetCenterPoint();
	for ( int i = 0; i < (int) m_points.size(); i++ )
	{
		Vec2 point = m_points[i];
		point -= centerPoint;
		point.RotateRadians( radian );
		m_points[i] = point + centerPoint;
	}
}

Vec2 Polygon2::Support( const Vec2& direction ) const
{
	float furthestDistance = 0.f;
	Vec2 furthestVertex = Vec2::ZERO;

	for( int i = 0; i < GetVertexCount(); ++i )
	{
		Vec2 vertexVec2 = GetPoints()[i];

		float distance = DotProduct2D( direction, vertexVec2 );
		if( distance > furthestDistance || i == 0 )
		{
			furthestDistance = distance;
			furthestVertex = vertexVec2;
		}
	}

	return furthestVertex;
}

int Polygon2::GetVertexCount() const
{
	return (int) m_points.size();
}

int Polygon2::GetEdgeCount() const
{
	return (int) m_points.size();
}

void Polygon2::GetEdge( int idx, Vec2* outStart, Vec2* outEnd )
{
	if ( GetEdgeCount() <= idx )
	{
		return;
	}

	if ( GetEdgeCount() - 1 == idx )
	{
		outStart = new Vec2( m_points[idx] );
		outEnd = new Vec2( m_points[0] );
	}
	else
	{
		outStart = new Vec2( m_points[idx] );
		outEnd = new Vec2( m_points[idx+1] );
	}
}

STATIC Polygon2 Polygon2::MakeFromLineLoop( Vec2 const* points, uint pointCount )
{
	// Start Creating Polygon
	Polygon2 polygon;
	for ( uint index = 0; index < pointCount; index++ )
	{
		Vec2 point = points[index];
		polygon.m_points.push_back(point);
	}
	polygon.OrderPointPositions();
	GUARANTEE_OR_DIE( polygon.IsConvex(), "Polygon is not a CONVEX!" );

	return polygon;
}

STATIC Polygon2 Polygon2::MakeConvexFromPointCloud( Vec2 const* points, uint pointCount )
{
	std::vector<Vec2> convexPointsList;
	// Find most left point
	Vec2 startPoint = points[0];
	for( uint index = 0; index < pointCount; index++ )
	{
		Vec2 point = points[index];
		if ( startPoint.x > point.x )
		{
			startPoint = point;
		}
	}

	// Try to find relative most left of current point
	convexPointsList.push_back( startPoint );
	Vec2 currentVertex = startPoint;
	do{
		Vec2 checkinPoint = points[0];
		for( uint index = 0; index < pointCount; index++ )
		{
			Vec2 point = points[index];
			if( point != currentVertex && CrossProduct2D( point - currentVertex, checkinPoint - currentVertex ) <= 0 )
			{
				checkinPoint = point;
			}
		}
		currentVertex = checkinPoint;
		if ( currentVertex != startPoint )
		{
			convexPointsList.push_back( checkinPoint );
		}
	}while( currentVertex != startPoint );

	// create polygon and ordering points by counter-clock wise
	Polygon2 polygon;
	for( int index = 0; index < (int) convexPointsList.size(); index++ )
	{
		Vec2 point = convexPointsList[index];
		polygon.m_points.push_back( point );
	}
	polygon.OrderPointPositions();
	GUARANTEE_OR_DIE( polygon.IsConvex(), "Polygon is not a CONVEX!" );


	return polygon;
}
