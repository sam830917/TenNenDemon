#pragma once
#include "Engine/Math/Vec2.hpp"
#include <vector>

typedef unsigned int uint;

class Polygon2
{
private:
	std::vector<Vec2> m_points;

public:
	~Polygon2();
	bool	IsValid() const; // must have at least 3 points to be considered a polygon

	bool	IsConvex() const;
	float	GetDistance( Vec2 point ) const;
	void	Translate( const Vec2& translation );
	Vec2	GetCenterPoint() const;
	void	OrderPointPositions();
	void	AddNewPoints( Vec2 const* points, uint pointCount );
	Vec2	GetPointOnNearestEdge( const Vec2& refPos ) const;
	void	RotateInRadian( float radian );
	Vec2	Support( const Vec2& direction ) const;

	// In MathUtils
	//bool	Contains( Vec2 point ) const;
	//Vec2	GetClosestPoint( Vec2 point ) const;

	// accessors
	int		GetVertexCount() const;
	int		GetEdgeCount() const;
	void	GetEdge( int idx, Vec2* outStart, Vec2* outEnd );
	std::vector<Vec2> GetPoints() const { return m_points; }

public: // static constructors (feel free to just use a constructor - I just like descriptive names)
		// in this case, these two take the same parameters but behave differently

	// construct from a counter-clockwise line loop
	static Polygon2 MakeFromLineLoop( Vec2 const* points, uint pointCount );

	// create a convex wrapping of a collection of points;  
	static Polygon2 MakeConvexFromPointCloud( Vec2 const* points, uint pointCount );
};