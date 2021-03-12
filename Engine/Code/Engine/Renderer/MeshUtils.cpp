#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Mikkt.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"


void AppendAABB2D( std::vector<Vertex_PCU>& verts, const AABB2& aabb2, const Rgba8& color )
{
	verts.push_back( Vertex_PCU( Vec3( aabb2.mins.x, aabb2.mins.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex_PCU( Vec3( aabb2.mins.x, aabb2.maxs.y, 0.f ), color, Vec2( 0.f, 1.f ) ) );
	verts.push_back( Vertex_PCU( Vec3( aabb2.maxs.x, aabb2.mins.y, 0.f ), color, Vec2( 1.f, 0.f ) ) );

	verts.push_back( Vertex_PCU( Vec3( aabb2.maxs.x, aabb2.mins.y, 0.f ), color, Vec2( 1.f, 0.f ) ) );
	verts.push_back( Vertex_PCU( Vec3( aabb2.mins.x, aabb2.maxs.y, 0.f ), color, Vec2( 0.f, 1.f ) ) );
	verts.push_back( Vertex_PCU( Vec3( aabb2.maxs.x, aabb2.maxs.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
}

void AppendTriangle2D( std::vector<Vertex_PCU>& verts, Vec2 p0, Vec2 p1, Vec2 p2, const Rgba8& color )
{
	verts.push_back( Vertex_PCU( Vec3( p0, 0.f ), color, Vec2::ZERO ) );
	verts.push_back( Vertex_PCU( Vec3( p1, 0.f ), color, Vec2::ZERO ) );
	verts.push_back( Vertex_PCU( Vec3( p2, 0.f ), color, Vec2::ZERO ) );
}

void AppendQuad( std::vector<Vertex_PCU>& verts, Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, AABB2 uvs, const Rgba8& color )
{
	// 2 --- 3
	// | \   |
	// |   \ |
	// 0 --- 1
	verts.push_back( Vertex_PCU( p0, color, uvs.mins ) );
	verts.push_back( Vertex_PCU( p1, color, Vec2( uvs.maxs.x, uvs.mins.y ) ) );
	verts.push_back( Vertex_PCU( p2, color, Vec2( uvs.mins.x, uvs.maxs.y ) ) );

	verts.push_back( Vertex_PCU( p1, color, Vec2( uvs.maxs.x, uvs.mins.y ) ) );
	verts.push_back( Vertex_PCU( p3, color, uvs.maxs ) );
	verts.push_back( Vertex_PCU( p2, color, Vec2( uvs.mins.x, uvs.maxs.y ) ) );
}

void AppendQuad( std::vector<Vertex_PCU>& verts, Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, AABB2 uvs, const Rgba8& color0, const Rgba8& color1, const Rgba8& color2, const Rgba8& color3 )
{
	verts.push_back( Vertex_PCU( p0, color0, uvs.mins ) );
	verts.push_back( Vertex_PCU( p1, color1, Vec2( uvs.maxs.x, uvs.mins.y ) ) );
	verts.push_back( Vertex_PCU( p2, color2, Vec2( uvs.mins.x, uvs.maxs.y ) ) );

	verts.push_back( Vertex_PCU( p1, color1, Vec2( uvs.maxs.x, uvs.mins.y ) ) );
	verts.push_back( Vertex_PCU( p3, color3, uvs.maxs ) );
	verts.push_back( Vertex_PCU( p2, color2, Vec2( uvs.mins.x, uvs.maxs.y ) ) );
}

void AppendCircle( std::vector<Vertex_PCU>& verts, Vec3 center, float radius, const Rgba8& color )
{
	const int vertexNumber = 64;
	Vec3 vertexsVec3[vertexNumber + 1];
	float degrees = 0.f;
	float degreesPerVertex = 360.f / vertexNumber;

	for( int vertexIndex = 0; vertexIndex < vertexNumber + 1; vertexIndex++ )
	{
		Vec3 newVec3 = Vec3( radius, 0.f, 0.f );
		vertexsVec3[vertexIndex] = newVec3.GetRotatedAboutZDegrees( degrees );
		degrees += degreesPerVertex;
	}

	// Create VertexPCU
	for( int triangleIndex = 0; triangleIndex < vertexNumber; triangleIndex++ )
	{
		if( triangleIndex + 1 == vertexNumber )
		{
			verts.push_back( Vertex_PCU( vertexsVec3[triangleIndex] + center, color, Vec2() ) );
			verts.push_back( Vertex_PCU( vertexsVec3[0] + center, color, Vec2() ) );
			verts.push_back( Vertex_PCU( center, color, Vec2() ) );
		}
		else
		{
			verts.push_back( Vertex_PCU( vertexsVec3[triangleIndex] + center, color, Vec2() ) );
			verts.push_back( Vertex_PCU( vertexsVec3[triangleIndex + 1] + center, color, Vec2() ) );
			verts.push_back( Vertex_PCU( center, color, Vec2() ) );
		}
	}
}

void AppendText3D( std::vector<Vertex_PCU>& verts, const std::string& text, const Vec3& position, float size, BitmapFont* font, const Rgba8& color )
{
	font->AddVertsForText3D( verts, position, size, text, color );
}

void AppendIndexedCubeToVerts( std::vector<Vertex_PCU>& verts, std::vector<uint>& indices, const AABB3& aabb3 )
{
	AppendIndexedCubeToVerts( verts, indices, aabb3, Rgba8::WHITE );
}

void AppendIndexedCubeToVerts( std::vector<Vertex_PCU>& verts, std::vector<uint>& indices, const AABB3& aabb3, const Rgba8& color )
{
	// 2 --- 3
	// | \   |
	// |   \ |
	// 0 --- 1
	// front side
	/*0*/verts.push_back( Vertex_PCU( Vec3( aabb3.mins.x, aabb3.mins.y, aabb3.maxs.z ), color, Vec2( 0.f, 0.f ) ) );
	/*1*/verts.push_back( Vertex_PCU( Vec3( aabb3.maxs.x, aabb3.mins.y, aabb3.maxs.z ), color, Vec2( 1.f, 0.f ) ) );
	/*2*/verts.push_back( Vertex_PCU( Vec3( aabb3.mins.x, aabb3.maxs.y, aabb3.maxs.z ), color, Vec2( 0.f, 1.f ) ) );
	/*3*/verts.push_back( Vertex_PCU( Vec3( aabb3.maxs.x, aabb3.maxs.y, aabb3.maxs.z ), color, Vec2( 1.f, 1.f ) ) );

	// 7 --- 5
	// |   / |
	// | /   |
	// 6 --- 4
	// back side
	/*4*/verts.push_back( Vertex_PCU( Vec3( aabb3.mins.x, aabb3.mins.y, aabb3.mins.z ), color, Vec2( 0.f, 1.f ) ) );
	/*5*/verts.push_back( Vertex_PCU( Vec3( aabb3.mins.x, aabb3.maxs.y, aabb3.mins.z ), color, Vec2( 0.f, 0.f ) ) );
	/*6*/verts.push_back( Vertex_PCU( Vec3( aabb3.maxs.x, aabb3.mins.y, aabb3.mins.z ), color, Vec2( 1.f, 1.f ) ) );
	/*7*/verts.push_back( Vertex_PCU( Vec3( aabb3.maxs.x, aabb3.maxs.y, aabb3.mins.z ), color, Vec2( 1.f, 0.f ) ) );

	/*8*/verts.push_back( Vertex_PCU( Vec3( aabb3.maxs.x, aabb3.mins.y, aabb3.mins.z ), color, Vec2( 0.f, 0.f ) ) );
	/*9*/verts.push_back( Vertex_PCU( Vec3( aabb3.maxs.x, aabb3.maxs.y, aabb3.mins.z ), color, Vec2( 0.f, 1.f ) ) );
	/*10*/verts.push_back( Vertex_PCU( Vec3( aabb3.mins.x, aabb3.maxs.y, aabb3.mins.z ), color, Vec2( 1.f, 1.f ) ) );
	/*11*/verts.push_back( Vertex_PCU( Vec3( aabb3.mins.x, aabb3.mins.y, aabb3.mins.z ), color, Vec2( 1.f, 0.f ) ) );

	uint indexArray[] =
	{
		0, 1, 2,  3, 2, 1,   // front
		4, 5, 6,  7, 6, 5,   // back
		2, 3, 7,  7, 5, 2,   // top
		1, 0, 6,  4, 6, 0,   // bottom
		0, 2, 10, 0, 10, 11, // left
		3, 1, 8,  3, 8, 9    // right
	};
	indices.insert( indices.end(), std::begin( indexArray ), std::end( indexArray ) );
}

void AppendUVSphereToIndexedVertexArray( std::vector<Vertex_PCU>& verts, std::vector<uint>& indices, Vec3 center, float radius, uint horizintalCuts, uint verticalCuts, const Rgba8& color )
{
	float verticalAngle = 0.f;
	float horizintalAngle = 0.f;

	const float addVerticalDegree = 180.f / (float) verticalCuts;
	const float addHorizintalDegree = 360.f / (float) horizintalCuts;

	for( uint verticalIndex = 0; verticalIndex <= verticalCuts; ++verticalIndex )
	{
		verticalAngle = 90.f - verticalIndex * addVerticalDegree;
		float xy = radius * CosDegrees( verticalAngle );
		float y = radius * SinDegrees( verticalAngle );

		for( uint horizintalIndex = 0; horizintalIndex <= horizintalCuts; ++horizintalIndex )
		{
			horizintalAngle = horizintalIndex * addHorizintalDegree;

			float x = -xy * CosDegrees(horizintalAngle);
			float z = xy * SinDegrees(horizintalAngle);
			Vec3 position = Vec3( x, y, z );

			float u = (float) horizintalIndex / (float) horizintalCuts;
			float v = (float) verticalIndex / (float) verticalCuts;
			verts.push_back( Vertex_PCU( position + center, color, Vec2( u, 1 - v ) ) );
		}
	}

	for( uint verticalIndex = 0; verticalIndex < verticalCuts; ++verticalIndex )
	{
		int k1 = verticalIndex * ( horizintalCuts + 1 );
		int k2 = k1 + horizintalCuts + 1;

		for( uint horizintalIndex = 0; horizintalIndex < horizintalCuts; ++horizintalIndex, ++k1, ++k2 )
		{
			if( verticalIndex != 0 )
			{
				indices.push_back( k1 );
				indices.push_back( k2 );
				indices.push_back( k1 + 1 );
			}
			// k1+1 => k2 => k2+1
			if( verticalIndex != (verticalCuts - 1) )
			{
				indices.push_back( k1 + 1 );
				indices.push_back( k2 );
				indices.push_back( k2 + 1 );
			}
		}
	}
}

void AppendCylindarToIndexedVertexArray( std::vector<Vertex_PCU>& verts, std::vector<uint>& indices, Vec3 point0, Vec3 point1, float radius, const Rgba8& startColor, const Rgba8& endColor )
{
	Mat44 mat = Mat44::LookAtRH( point0, point1 );
	Vec3 ibasis = mat.GetIBasis3D();
	Vec3 jbasis = mat.GetJBasis3D();

	const int vertexNumber = 64;
	float degrees = 0.f;
	float degreesPerVertex = 360.f / vertexNumber;

	// Start Circle
	for( int vertexIndex = 0; vertexIndex < vertexNumber; vertexIndex++ )
	{
		Vec3 newVec3 = point0 + (ibasis * CosDegrees( degrees ) + jbasis * SinDegrees( degrees )) * radius;
		verts.push_back( Vertex_PCU( newVec3, startColor, Vec2::ZERO ) );

		degrees += degreesPerVertex;
	}

	// End Circle
	degrees = 0.f;
	for( int vertexIndex = 0; vertexIndex < vertexNumber; vertexIndex++ )
	{
		Vec3 newVec3 = point1 + (ibasis * CosDegrees( degrees ) + jbasis * SinDegrees( degrees )) * radius;
		verts.push_back( Vertex_PCU( newVec3, endColor, Vec2::ZERO ) );

		degrees += degreesPerVertex;
	}

	// circle
	for ( int i = 0; i < vertexNumber - 2; ++i )
	{
		indices.push_back( 0 );
		indices.push_back( i + 1 );
		indices.push_back( i + 2 );
		indices.push_back( vertexNumber );
		indices.push_back( i + 2 + vertexNumber );
		indices.push_back( i + 1 + vertexNumber );
	}

	// wall
	for ( int i = 0; i < vertexNumber; ++i )
	{
		if ( i == vertexNumber - 1 )
		{
			indices.push_back( i );
			indices.push_back( vertexNumber );
			indices.push_back( 0 );

			indices.push_back( vertexNumber );
			indices.push_back( i );
			indices.push_back( i + vertexNumber );
		}
		else
		{
			indices.push_back( i );
			indices.push_back( i + 1 + vertexNumber );
			indices.push_back( i + 1 );

			indices.push_back( i + 1 + vertexNumber );
			indices.push_back( i );
			indices.push_back( i + vertexNumber );
		}
	}
}

void AppendCylindarToIndexedVertexArray( std::vector<Vertex_PCU>& verts, std::vector<uint>& indices, Vec3 point0, Vec3 point1, float radius, const Rgba8& color )
{
	AppendCylindarToIndexedVertexArray( verts, indices, point0, point1, radius, color, color );
}

void AppendConeToIndexedVertexArray( std::vector<Vertex_PCU>& verts, std::vector<uint>& indices, Vec3 circleCenter, Vec3 endPoint, float radius, const Rgba8& color )
{
	Mat44 mat = Mat44::LookAtRH( circleCenter, endPoint );
	Vec3 ibasis = mat.GetIBasis3D();
	Vec3 jbasis = mat.GetJBasis3D();

	const int vertexNumber = 64;
	float degrees = 0.f;
	float degreesPerVertex = 360.f / vertexNumber;

	// Circle
	for( int vertexIndex = 0; vertexIndex < vertexNumber; vertexIndex++ )
	{
		Vec3 newVec3 = circleCenter + (ibasis * CosDegrees( degrees ) + jbasis * SinDegrees( degrees )) * radius;
		verts.push_back( Vertex_PCU( newVec3, color, Vec2::ZERO ) );

		degrees += degreesPerVertex;
	}
	verts.push_back( Vertex_PCU( endPoint, color, Vec2::ZERO ) );

	// circle
	for( int i = 0; i < vertexNumber - 2; ++i )
	{
		indices.push_back( 0 );
		indices.push_back( i + 1 );
		indices.push_back( i + 2 );
	}
	for( int i = 0; i < vertexNumber; ++i )
	{
		if ( i == vertexNumber - 1 )
		{
			indices.push_back( vertexNumber );
			indices.push_back( 0 );
			indices.push_back( i );
		}
		else
		{
			indices.push_back( vertexNumber );
			indices.push_back( i + 1 );
			indices.push_back( i );
		}
	}
}

void AddCylindarToIndexedVertexArray( std::vector<Vertex_PCU>& verts, std::vector<uint>& indices, Vec3 point0, Vec3 point1, float radius, const Rgba8& startColor, const Rgba8& endColor )
{
	Mat44 mat = Mat44::LookAtRH( point0, point1 );
	Vec3 ibasis = mat.GetIBasis3D();
	Vec3 jbasis = mat.GetJBasis3D();

	const int vertexOriginSize = (int)verts.size();
	const int vertexNumber = 64;
	float degrees = 0.f;
	float degreesPerVertex = 360.f / vertexNumber;

	// Start Circle
	for( int vertexIndex = 0; vertexIndex < vertexNumber; vertexIndex++ )
	{
		Vec3 newVec3 = point0 + (ibasis * CosDegrees( degrees ) + jbasis * SinDegrees( degrees )) * radius;
		verts.push_back( Vertex_PCU( newVec3, startColor, Vec2::ZERO ) );

		degrees += degreesPerVertex;
	}

	// End Circle
	degrees = 0.f;
	for( int vertexIndex = 0; vertexIndex < vertexNumber; vertexIndex++ )
	{
		Vec3 newVec3 = point1 + (ibasis * CosDegrees( degrees ) + jbasis * SinDegrees( degrees )) * radius;
		verts.push_back( Vertex_PCU( newVec3, endColor, Vec2::ZERO ) );

		degrees += degreesPerVertex;
	}

	// circle
	for( int i = 0; i < vertexNumber - 2; ++i )
	{
		indices.push_back( 0 );
		indices.push_back( i + 1 );
		indices.push_back( i + 2 );
		indices.push_back( vertexNumber );
		indices.push_back( i + 2 + vertexNumber );
		indices.push_back( i + 1 + vertexNumber );
	}

	// wall
	for( int i = 0; i < vertexNumber; ++i )
	{
		if( i == vertexNumber - 1 )
		{
			indices.push_back( i + vertexOriginSize );
			indices.push_back( vertexNumber + vertexOriginSize );
			indices.push_back( vertexOriginSize );

			indices.push_back( vertexNumber + vertexOriginSize );
			indices.push_back( i + vertexOriginSize );
			indices.push_back( i + vertexNumber + vertexOriginSize );
		}
		else
		{
			indices.push_back( i + vertexOriginSize );
			indices.push_back( i + 1 + vertexNumber + vertexOriginSize );
			indices.push_back( i + 1 + vertexOriginSize );

			indices.push_back( i + 1 + vertexNumber + vertexOriginSize );
			indices.push_back( i + vertexOriginSize );
			indices.push_back( i + vertexNumber + vertexOriginSize );
		}
	}
}

void AddConeToIndexedVertexArray( std::vector<Vertex_PCU>& verts, std::vector<uint>& indices, Vec3 circleCenter, Vec3 endPoint, float radius, const Rgba8& color )
{
	Mat44 mat = Mat44::LookAtRH( circleCenter, endPoint );
	Vec3 ibasis = mat.GetIBasis3D();
	Vec3 jbasis = mat.GetJBasis3D();

	const int vertexOriginSize = (int) verts.size();
	const int vertexNumber = 64;
	float degrees = 0.f;
	float degreesPerVertex = 360.f / vertexNumber;

	// Circle
	for( int vertexIndex = 0; vertexIndex < vertexNumber; vertexIndex++ )
	{
		Vec3 newVec3 = circleCenter + (ibasis * CosDegrees( degrees ) + jbasis * SinDegrees( degrees )) * radius;
		verts.push_back( Vertex_PCU( newVec3, color, Vec2::ZERO ) );

		degrees += degreesPerVertex;
	}
	verts.push_back( Vertex_PCU( endPoint, color, Vec2::ZERO ) );

	// circle
	for( int i = 0; i < vertexNumber - 2; ++i )
	{
		indices.push_back( vertexOriginSize );
		indices.push_back( i + 1 + vertexOriginSize );
		indices.push_back( i + 2 + vertexOriginSize );
	}
	for( int i = 0; i < vertexNumber; ++i )
	{
		if( i == vertexNumber - 1 )
		{
			indices.push_back( vertexNumber + vertexOriginSize );
			indices.push_back( vertexOriginSize );
			indices.push_back( i + vertexOriginSize );
		}
		else
		{
			indices.push_back( vertexNumber + vertexOriginSize );
			indices.push_back( i + 1 + vertexOriginSize );
			indices.push_back( i + vertexOriginSize );
		}
	}
}

void ConvertPCUTBNToPCU( std::vector<Vertex_PCU>& output, std::vector<Vertex_PCUTBN> const& input )
{
	for ( int i = 0; i < (int) input.size(); ++i )
	{
		const Vertex_PCUTBN& vertex = input[i];
		output.push_back( Vertex_PCU( vertex.m_position, vertex.m_color, vertex.m_uvTexCoords ) );
	}
}

void AppendQuad( std::vector<Vertex_PCUTBN>& out, Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, AABB2 uvs, const Rgba8& color )
{
	Vec3 normal = CrossProduct3D(p2 - p0, p1 - p0).GetNormalized();
	Vec3 tangent = ( p2 - p0 ).GetNormalized();
	Vec3 bitangent = ( p1 - p0 ).GetNormalized();

	out.push_back( Vertex_PCUTBN( p0, color, uvs.mins, tangent, bitangent, normal ) );
	out.push_back( Vertex_PCUTBN( p1, color, Vec2( uvs.maxs.x, uvs.mins.y ), tangent, bitangent, normal ) );
	out.push_back( Vertex_PCUTBN( p2, color, Vec2( uvs.mins.x, uvs.maxs.y ), tangent, bitangent, normal ) );

	out.push_back( Vertex_PCUTBN( p1, color, Vec2( uvs.maxs.x, uvs.mins.y ), tangent, bitangent, normal ) );
	out.push_back( Vertex_PCUTBN( p3, color, uvs.maxs, tangent, bitangent, normal ) );
	out.push_back( Vertex_PCUTBN( p2, color, Vec2( uvs.mins.x, uvs.maxs.y ), tangent, bitangent, normal ) );
}

void AppendUVSphereToIndexedVertexArray( std::vector<Vertex_PCUTBN>& verts, std::vector<uint>& indices, Vec3 center, float radius, uint horizintalCuts, uint verticalCuts, const Rgba8& color )
{
	float verticalAngle = 0.f;
	float horizintalAngle = 0.f;

	const float addVerticalDegree = 180.f / (float)verticalCuts;
	const float addHorizintalDegree = 360.f / (float)horizintalCuts;

	for( uint verticalIndex = 0; verticalIndex <= verticalCuts; ++verticalIndex )
	{
		verticalAngle = 90.f - verticalIndex * addVerticalDegree;
		float xy = radius * CosDegrees( verticalAngle );
		float y = radius * SinDegrees( verticalAngle );

		for( uint horizintalIndex = 0; horizintalIndex <= horizintalCuts; ++horizintalIndex )
		{
			horizintalAngle = horizintalIndex * addHorizintalDegree;

			float x = xy * CosDegrees( horizintalAngle );
			float z = -xy * SinDegrees( horizintalAngle );
			Vec3 position = Vec3( x, y, z ) + center;

			float u = (float)horizintalIndex / (float)horizintalCuts;
			float v = (float)verticalIndex / (float)verticalCuts;

			Vec3 normal = Vec3( x, y, z ).GetNormalized();
			Vec3 tangent = Vec3( -SinDegrees( horizintalAngle ), 0.f, -CosDegrees( horizintalAngle ) );
			Vec3 bitangent = CrossProduct3D( normal, tangent );

			verts.push_back( Vertex_PCUTBN( position, color, Vec2( u, 1 - v ), tangent, bitangent, normal ) );
		}
	}

	for( uint verticalIndex = 0; verticalIndex < verticalCuts; ++verticalIndex )
	{
		int k1 = verticalIndex * (horizintalCuts + 1);
		int k2 = k1 + horizintalCuts + 1;

		for( uint horizintalIndex = 0; horizintalIndex < horizintalCuts; ++horizintalIndex, ++k1, ++k2 )
		{
			if( verticalIndex != 0 )
			{
				indices.push_back( k1 );
				indices.push_back( k2 );
				indices.push_back( k1 + 1 );
			}
			// k1+1 => k2 => k2+1
			if( verticalIndex != (verticalCuts - 1) )
			{
				indices.push_back( k1 + 1 );
				indices.push_back( k2 );
				indices.push_back( k2 + 1 );
			}
		}
	}
}

void AppendQuad( std::vector<Vertex_PCUTBN>& out, std::vector<uint>& indices, Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, AABB2 uvs, const Rgba8& color )
{
	Vec3 normal = CrossProduct3D( p1 - p0, p2 - p0 ).GetNormalized();
	Vec3 tangent = (p1 - p0).GetNormalized();
	Vec3 bitangent = (p2 - p0).GetNormalized();

	const uint size = (uint) out.size();
	out.push_back( Vertex_PCUTBN( p0, color, uvs.mins, tangent, bitangent, normal ) );
	out.push_back( Vertex_PCUTBN( p1, color, Vec2( uvs.maxs.x, uvs.mins.y ), tangent, bitangent, normal ) );
	out.push_back( Vertex_PCUTBN( p2, color, Vec2( uvs.mins.x, uvs.maxs.y ), tangent, bitangent, normal ) );
	out.push_back( Vertex_PCUTBN( p3, color, uvs.maxs, tangent, bitangent, normal ) );
	//out.push_back( Vertex_PCUTBN( p0, color, Vec2( uvs.maxs.x, uvs.mins.y ), tangent, bitangent, normal ) );
	//out.push_back( Vertex_PCUTBN( p1, color, uvs.maxs, tangent, bitangent, normal ) );
	//out.push_back( Vertex_PCUTBN( p2, color, uvs.mins, tangent, bitangent, normal ) );
	//out.push_back( Vertex_PCUTBN( p3, color, Vec2( uvs.mins.x, uvs.maxs.y ), tangent, bitangent, normal ) );

	uint indexArray[] =
	{
		0+size, 1+size, 2+size,   1+size, 3+size, 2+size
	};
	indices.insert( indices.end(), std::begin( indexArray ), std::end( indexArray ) );
}

void AppendIndexedCubeToVerts( std::vector<Vertex_PCUTBN>& verts, std::vector<uint>& indices, const AABB3& aabb3, const Rgba8& color )
{
	// Front
	//AppendQuad( verts, indices, Vec3( aabb3.mins.x, aabb3.mins.y, aabb3.maxs.z ), Vec3( aabb3.maxs.x, aabb3.mins.y, aabb3.maxs.z ),
	//	Vec3( aabb3.mins.x, aabb3.maxs.y, aabb3.maxs.z ), Vec3( aabb3.maxs.x, aabb3.maxs.y, aabb3.maxs.z ), AABB2::ZERO_TO_ONE, color );
	//
	//// Top
	//AppendQuad( verts, indices, Vec3( aabb3.mins.x, aabb3.maxs.y, aabb3.maxs.z ), Vec3( aabb3.maxs.x, aabb3.maxs.y, aabb3.maxs.z ),
	//	Vec3( aabb3.mins.x, aabb3.maxs.y, aabb3.mins.z ), Vec3( aabb3.maxs.x, aabb3.maxs.y, aabb3.mins.z ), AABB2::ZERO_TO_ONE, color );
	//
	//// Right
	//AppendQuad( verts, indices, Vec3( aabb3.mins.x, aabb3.mins.y, aabb3.mins.z ), Vec3( aabb3.mins.x, aabb3.mins.y, aabb3.maxs.z ),
	//	Vec3( aabb3.mins.x, aabb3.maxs.y, aabb3.mins.z ), Vec3( aabb3.mins.x, aabb3.maxs.y, aabb3.maxs.z ), AABB2::ZERO_TO_ONE, color );
	//
	//// Left
	//AppendQuad( verts, indices, Vec3( aabb3.maxs.x, aabb3.mins.y, aabb3.maxs.z ), Vec3( aabb3.maxs.x, aabb3.mins.y, aabb3.mins.z ),
	//	Vec3( aabb3.maxs.x, aabb3.maxs.y, aabb3.maxs.z ), Vec3( aabb3.maxs.x, aabb3.maxs.y, aabb3.mins.z ), AABB2::ZERO_TO_ONE, color );
	//
	//// Bottom
	//AppendQuad( verts, indices, Vec3( aabb3.mins.x, aabb3.mins.y, aabb3.mins.z ), Vec3( aabb3.maxs.x, aabb3.mins.y, aabb3.mins.z ),
	//	Vec3( aabb3.mins.x, aabb3.mins.y, aabb3.maxs.z ), Vec3( aabb3.maxs.x, aabb3.mins.y, aabb3.maxs.z ), AABB2::ZERO_TO_ONE, color );
	//
	//// Back
	//AppendQuad( verts, indices, Vec3( aabb3.maxs.x, aabb3.mins.y, aabb3.mins.z ), Vec3( aabb3.mins.x, aabb3.mins.y, aabb3.mins.z ),
	//	Vec3( aabb3.maxs.x, aabb3.maxs.y, aabb3.mins.z ), Vec3( aabb3.mins.x, aabb3.maxs.y, aabb3.mins.z ), AABB2::ZERO_TO_ONE, color );

	// Front
	AppendQuad( verts, indices, Vec3( aabb3.mins.x, aabb3.maxs.y, aabb3.mins.z ), Vec3( aabb3.mins.x, aabb3.mins.y, aabb3.mins.z ),
		Vec3( aabb3.mins.x, aabb3.maxs.y, aabb3.maxs.z ), Vec3( aabb3.mins.x, aabb3.mins.y, aabb3.maxs.z ), AABB2::ZERO_TO_ONE, color );
	
	// Top
	AppendQuad( verts, indices, Vec3( aabb3.mins.x, aabb3.mins.y, aabb3.maxs.z ), Vec3( aabb3.maxs.x, aabb3.mins.y, aabb3.maxs.z ),
		Vec3( aabb3.mins.x, aabb3.maxs.y, aabb3.maxs.z ), Vec3( aabb3.maxs.x, aabb3.maxs.y, aabb3.maxs.z ), AABB2::ZERO_TO_ONE, color );
	
	// Left
	AppendQuad( verts, indices, Vec3( aabb3.maxs.x, aabb3.maxs.y, aabb3.mins.z ), Vec3( aabb3.mins.x, aabb3.maxs.y, aabb3.mins.z ),
		Vec3( aabb3.maxs.x, aabb3.maxs.y, aabb3.maxs.z ), Vec3( aabb3.mins.x, aabb3.maxs.y, aabb3.maxs.z ), AABB2::ZERO_TO_ONE, color );
	
	// Right
	AppendQuad( verts, indices, Vec3( aabb3.mins.x, aabb3.mins.y, aabb3.mins.z ), Vec3( aabb3.maxs.x, aabb3.mins.y, aabb3.mins.z ),
		Vec3( aabb3.mins.x, aabb3.mins.y, aabb3.maxs.z ), Vec3( aabb3.maxs.x, aabb3.mins.y, aabb3.maxs.z ), AABB2::ZERO_TO_ONE, color );
	
	// Bottom
	AppendQuad( verts, indices, Vec3( aabb3.mins.x, aabb3.maxs.y, aabb3.mins.z ), Vec3( aabb3.maxs.x, aabb3.maxs.y, aabb3.mins.z ),
		Vec3( aabb3.mins.x, aabb3.mins.y, aabb3.mins.z ), Vec3( aabb3.maxs.x, aabb3.mins.y, aabb3.mins.z ), AABB2::ZERO_TO_ONE, color );
	
	// Back
	AppendQuad( verts, indices, Vec3( aabb3.maxs.x, aabb3.mins.y, aabb3.mins.z ), Vec3( aabb3.maxs.x, aabb3.maxs.y, aabb3.mins.z ),
		Vec3( aabb3.maxs.x, aabb3.mins.y, aabb3.maxs.z ), Vec3( aabb3.maxs.x, aabb3.maxs.y, aabb3.maxs.z ), AABB2::ZERO_TO_ONE, color );
}

void LoadOBJToVertexArray( std::vector<Vertex_PCUTBN>& vertices, char const* filename, mesh_import_options_t const& options )
{
	std::string fileStr = FileRend( std::string( filename ) );
	Strings objFile = SplitStringOnDelimiter( fileStr, '\n' );

	std::vector<Vec3> tempVertices;
	std::vector<Vec2> tempUVs;
	std::vector<Vec3> tempNormals;
	for( int i = 0; i < (int)objFile.size(); ++i )
	{
		std::string currentLineTexts = objFile[i];
		if( currentLineTexts.find( "v " ) == 0 )
		{
			Vec3 vertex;
			Strings strings = SplitStringOnDelimiterWithoutEmpty( currentLineTexts, ' ' );
			if ( (int) strings.size() != 4 )
			{
				ERROR_AND_DIE( Stringf( "\"%s\" has a error in line %i.", std::string( filename ).c_str(), i ) );
			}
			 
			vertex.SetFromText( Stringf( "%s,%s,%s", strings[1].c_str(), strings[2].c_str(), strings[3].c_str() ).c_str() );
			vertex = options.transform.TransformPosition3D( vertex );
			tempVertices.push_back( vertex );
		}
		else if( currentLineTexts.find( "vn " ) == 0 )
		{
			Vec3 normal;
			Strings strings = SplitStringOnDelimiterWithoutEmpty( currentLineTexts, ' ' );
			if( (int)strings.size() != 4 )
			{
				ERROR_AND_DIE( Stringf( "\"%s\" has a error in line %i.", std::string( filename ).c_str(), i ) );
			}

			normal.SetFromText( Stringf( "%s,%s,%s", strings[1].c_str(), strings[2].c_str(), strings[3].c_str() ).c_str() );
			normal = options.transform.TransformVector3D( normal );
			tempNormals.push_back( normal );
		}
		else if( currentLineTexts.find( "vt " ) == 0 )
		{
			Vec2 uv;
			Strings strings = SplitStringOnDelimiterWithoutEmpty( currentLineTexts, ' ' );
			if( (int)strings.size() != 3 && (int)strings.size() != 4 )
			{
				ERROR_AND_DIE( Stringf( "\"%s\" has a error in line %i.", std::string( filename ).c_str(), i ) );
			}

			uv.SetFromText( Stringf( "%s,%s", strings[1].c_str(), strings[2].c_str() ).c_str() );
			if ( options.invert_v )
			{
				uv = Vec2( uv.x, 1.f - uv.y );
			}
			tempUVs.push_back( uv );
		}
		else if( currentLineTexts.find( "f " ) == 0 )
		{
			Strings strings = SplitStringOnDelimiterWithoutEmpty( currentLineTexts, ' ' );
			if( (int)strings.size() < 4 )
			{
				ERROR_AND_DIE( Stringf( "\"%s\" has a error in line %i.", std::string( filename ).c_str(), i ) );
			}

			int stringSize = (int) strings.size();
			uint vertexIndexes[4];
			Vertex_PCUTBN vertexArray[4];
			bool recalculateNormal = true;
			for( int j = 1; j < stringSize; ++j )
			{
				Strings vertexStr = SplitStringOnDelimiter( strings[j], '/' );
				int vertexIndex = GetIntFromString( &vertexStr[0][0] ) - 1;
				int uvIndex = GetIntFromString( &vertexStr[1][0] ) - 1;
				Vec3 normal = Vec3::ZERO;
				if ( (int) vertexStr.size() == 3 )
				{
					int normalIndex = GetIntFromString( &vertexStr[2][0] ) - 1;
					normal = tempNormals[normalIndex];
					recalculateNormal = false;
				}

				vertexIndexes[j-1] = (uint) vertices.size();
				vertexArray[j-1] = Vertex_PCUTBN( tempVertices[vertexIndex], Rgba8::WHITE, tempUVs[uvIndex], Vec3::ZERO, Vec3::ZERO, normal );
			}

			// Recalculate normal
			Vertex_PCUTBN pcutbnArray[] =
			{
				vertexArray[0], vertexArray[1], vertexArray[2]
			};
			if( options.invert_winding_order )
			{
				Vertex_PCUTBN temp = pcutbnArray[1];
				pcutbnArray[1] = pcutbnArray[2];
				pcutbnArray[2] = temp;
			}
			if( options.generate_normals || recalculateNormal )
			{
				if ( options.invert_winding_order )
				{
					GenerateNormal( pcutbnArray[0], pcutbnArray[2], pcutbnArray[1], options.invert_winding_order );
				}
				else
				{
					GenerateNormal( pcutbnArray[0], pcutbnArray[1], pcutbnArray[2], options.invert_winding_order );
				}
			}
			vertices.insert( vertices.end(), std::begin( pcutbnArray ), std::end( pcutbnArray ) );

			// Fourth vertices
			if ( stringSize == 5 )
			{
				Vertex_PCUTBN pcutbnArray2[] =
				{
					vertexArray[0], vertexArray[2], vertexArray[3]
				};
				if( options.invert_winding_order )
				{
					Vertex_PCUTBN temp = pcutbnArray2[1];
					pcutbnArray2[1] = pcutbnArray2[2];
					pcutbnArray2[2] = temp;
				}
				if( options.generate_normals || recalculateNormal )
				{
					if( options.invert_winding_order )
					{
						GenerateNormal( pcutbnArray2[0], pcutbnArray2[2], pcutbnArray2[3], options.invert_winding_order );
					}
					else
					{
						GenerateNormal( pcutbnArray2[0], pcutbnArray2[3], pcutbnArray2[2], options.invert_winding_order );
					}
				}
				vertices.insert( vertices.end(), std::begin( pcutbnArray2 ), std::end( pcutbnArray2 ) );
			}
		}
	}
	// Set tangent and bitangent
	if ( options.generate_tangents )
	{
		GenerateTangentsForVertexArray( vertices );
	}
}
