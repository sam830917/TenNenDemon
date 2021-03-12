#include "GameCommon.hpp"
#include "Game.hpp"

void DrawLine( const Vec2& startVec2, const Vec2& endVec2, const Rgba8& color, float thickness )
{
	const float r = thickness * 0.5f;

	Vec2 fwd = endVec2 - startVec2;
	fwd.SetLength( r );
	Vec2 leftDisplacement = fwd.GetRotated90Degrees();

	const Vec2 endLeft = endVec2 + fwd + leftDisplacement;
	const Vec2 endRight = endVec2 + fwd - leftDisplacement;
	const Vec2 startLeft = startVec2 - fwd + leftDisplacement;
	const Vec2 startRight = startVec2 - fwd - leftDisplacement;

	Vertex_PCU lineVertexex[] = 
	{
		Vertex_PCU( Vec3( startLeft.x, startLeft.y, 0.f ), color, Vec2() ),
		Vertex_PCU( Vec3( startRight.x, startRight.y, 0.f ), color, Vec2() ),
		Vertex_PCU( Vec3( endLeft.x, endLeft.y, 0.f ), color, Vec2() ),

		Vertex_PCU( Vec3( endRight.x, endRight.y, 0.f ), color, Vec2() ),
		Vertex_PCU( Vec3( startRight.x, startRight.y, 0.f ), color, Vec2() ),
		Vertex_PCU( Vec3( endLeft.x, endLeft.y, 0.f ), color, Vec2() )
	};
	constexpr int NUM_VERTEXES = sizeof( lineVertexex ) / sizeof( lineVertexex[0] );

 	g_theRenderer->DrawVertexArray( NUM_VERTEXES, lineVertexex );
}

void DrawRing( const Vec2& centerVec2, float radius, const Rgba8& color, float thickness )
{
	const int verTexesNum = 100;
	Vec2 innerCircleArray[ verTexesNum ];
	Vec2 outterCircleArray[ verTexesNum ];
	const float r = thickness * 0.5f;

	float degrees = 0;
	for ( int vertexIndex = 0;vertexIndex < verTexesNum;vertexIndex++ )
	{
		Vec2 innerCircle = TransformPosition2D( Vec2( radius - r, 0.f ), 1.f, degrees, centerVec2 );
		Vec2 outterCircle = TransformPosition2D( Vec2( radius + r, 0.f ), 1.f, degrees, centerVec2 );
		innerCircleArray[ vertexIndex ] = innerCircle;
		outterCircleArray[ vertexIndex ] = outterCircle;
		degrees += 360.f / verTexesNum;
	}

	Vertex_PCU ringVertexes[ verTexesNum * 6 ];

	for( int vertexIndex = 0; vertexIndex < verTexesNum; vertexIndex++ )
	{
		const int lineVertexIdx = vertexIndex * 6;

		ringVertexes[lineVertexIdx] = Vertex_PCU( Vec3( innerCircleArray[vertexIndex].x, innerCircleArray[vertexIndex].y, 0.f ), color, Vec2() );
		if ( (vertexIndex + 1) == verTexesNum )
		{
			ringVertexes[lineVertexIdx + 1] = Vertex_PCU( Vec3( innerCircleArray[0].x, innerCircleArray[0].y, 0.f ), color, Vec2() );
			ringVertexes[lineVertexIdx + 2] = Vertex_PCU( Vec3( outterCircleArray[0].x, outterCircleArray[0].y, 0.f ), color, Vec2() );

			ringVertexes[lineVertexIdx + 3] = Vertex_PCU( Vec3( outterCircleArray[0].x, outterCircleArray[0].y, 0.f ), color, Vec2() );
		}
		else
		{
			ringVertexes[lineVertexIdx + 1] = Vertex_PCU( Vec3( innerCircleArray[vertexIndex + 1].x, innerCircleArray[vertexIndex + 1].y, 0.f ), color, Vec2() );
			ringVertexes[lineVertexIdx + 2] = Vertex_PCU( Vec3( outterCircleArray[vertexIndex + 1].x, outterCircleArray[vertexIndex + 1].y, 0.f ), color, Vec2() );

			ringVertexes[lineVertexIdx + 3] = Vertex_PCU( Vec3( outterCircleArray[vertexIndex + 1].x, outterCircleArray[vertexIndex + 1].y, 0.f ), color, Vec2() );
		}
		ringVertexes[lineVertexIdx + 4] = Vertex_PCU( Vec3( outterCircleArray[vertexIndex].x, outterCircleArray[vertexIndex].y, 0.f ), color, Vec2() );
		ringVertexes[lineVertexIdx + 5] = Vertex_PCU( Vec3( innerCircleArray[vertexIndex].x, innerCircleArray[vertexIndex].y, 0.f ), color, Vec2() );
	}
	constexpr int NUM_VERTEXES = sizeof( ringVertexes ) / sizeof( ringVertexes[0] );

	g_theRenderer->DrawVertexArray( NUM_VERTEXES, ringVertexes );
}

void AppendVertsForAABB2D( std::vector<Vertex_PCU>& vertVector, const AABB2& localBounds, const Rgba8& color )
{
	Vec2 minVec2 = localBounds.mins;
	Vec2 maxVec2 = localBounds.maxs;

	vertVector.push_back( Vertex_PCU( Vec3( minVec2.x, minVec2.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	vertVector.push_back( Vertex_PCU( Vec3( minVec2.x, maxVec2.y, 0.f ), color, Vec2( 0.f, 1.f ) ) );
	vertVector.push_back( Vertex_PCU( Vec3( maxVec2.x, minVec2.y, 0.f ), color, Vec2( 1.f, 0.f ) ) );
	
	vertVector.push_back( Vertex_PCU( Vec3( maxVec2.x, minVec2.y, 0.f ), color, Vec2( 1.f, 0.f ) ) );
	vertVector.push_back( Vertex_PCU( Vec3( minVec2.x, maxVec2.y, 0.f ), color, Vec2( 0.f, 1.f ) ) );
	vertVector.push_back( Vertex_PCU( Vec3( maxVec2.x, maxVec2.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
}

void AddVertsForAABB2( std::vector<Vertex_PCU>& verts, const AABB2& localBounds, const Rgba8& color, const Vec2& uvMins, const Vec2& uvMaxs )
{
	Vec2 minVec2 = localBounds.mins;
	Vec2 maxVec2 = localBounds.maxs;

	verts.push_back( Vertex_PCU( Vec3( minVec2.x, minVec2.y, 0.f ), color, Vec2( uvMins.x, uvMins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( minVec2.x, maxVec2.y, 0.f ), color, Vec2( uvMins.x, uvMaxs.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( maxVec2.x, minVec2.y, 0.f ), color, Vec2( uvMaxs.x, uvMins.y ) ) );

	verts.push_back( Vertex_PCU( Vec3( maxVec2.x, minVec2.y, 0.f ), color, Vec2( uvMaxs.x, uvMins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( minVec2.x, maxVec2.y, 0.f ), color, Vec2( uvMins.x, uvMaxs.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( maxVec2.x, maxVec2.y, 0.f ), color, Vec2( uvMaxs.x, uvMaxs.y ) ) );
}
