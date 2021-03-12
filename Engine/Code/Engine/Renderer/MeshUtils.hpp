#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/Mat44.hpp"
#include <string>
#include <vector>

struct mesh_import_options_t
{
	Mat44 transform            = Mat44::IDENTITY; // what space transform to apply to the object

	bool invert_v              = false;    // Required - inverts V coordinate (v = 1.0f - v)
	bool invert_winding_order  = false;    // Required - Change the winding order of all faces
	bool generate_normals      = false;    // Required - Generate normals for the surface if they weren't in the file
	bool generate_tangents     = false;    // Required - Generate tangents for the surface if they weren't in the file
	bool clean                 = false;    // Extra    - Convert a vertex array to an index vertex array by removing duplicates
};

//------------------------------------------------------------------------------------------------------------------------
//												Vertex PCU functions
//------------------------------------------------------------------------------------------------------------------------
void AppendAABB2D( std::vector<Vertex_PCU>& verts, const AABB2& aabb2, const Rgba8& color );
void AppendTriangle2D( std::vector<Vertex_PCU>& verts, Vec2 p0, Vec2 p1, Vec2 p2, const Rgba8& color );
void AppendQuad( std::vector<Vertex_PCU>& verts, Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, AABB2 uvs, const Rgba8& color );
void AppendQuad( std::vector<Vertex_PCU>& verts, Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, AABB2 uvs, const Rgba8& color0, const Rgba8& color1, const Rgba8& color2, const Rgba8& color3 );
void AppendCircle( std::vector<Vertex_PCU>& verts, Vec3 center, float radius, const Rgba8& color );
void AppendText3D( std::vector<Vertex_PCU>& verts, const std::string& text, const Vec3& position, float size, BitmapFont* font, const Rgba8& color );
void AppendIndexedCubeToVerts( std::vector<Vertex_PCU>& verts, std::vector<uint>& indices, const AABB3& aabb3 );
void AppendIndexedCubeToVerts( std::vector<Vertex_PCU>& verts, std::vector<uint>& indices, const AABB3& aabb3, const Rgba8& color );
void AppendUVSphereToIndexedVertexArray( std::vector<Vertex_PCU>& verts, std::vector<uint>& indices, Vec3 center, float radius, uint horizintalCuts, uint verticalCuts, const Rgba8& color );
void AppendCylindarToIndexedVertexArray( std::vector<Vertex_PCU>& verts, std::vector<uint>& indices, Vec3 point0, Vec3 point1, float radius, const Rgba8& startColor, const Rgba8& endColor );
void AppendCylindarToIndexedVertexArray( std::vector<Vertex_PCU>& verts, std::vector<uint>& indices, Vec3 point0, Vec3 point1, float radius, const Rgba8& color );
void AppendConeToIndexedVertexArray( std::vector<Vertex_PCU>& verts, std::vector<uint>& indices, Vec3 circleCenter, Vec3 endPoint, float radius, const Rgba8& color );

//------------------------------------------------------------------------------------------------------------------------
//												Add vertex functions
//------------------------------------------------------------------------------------------------------------------------
void AddCylindarToIndexedVertexArray( std::vector<Vertex_PCU>& verts, std::vector<uint>& indices, Vec3 point0, Vec3 point1, float radius, const Rgba8& startColor, const Rgba8& endColor );
void AddConeToIndexedVertexArray( std::vector<Vertex_PCU>& verts, std::vector<uint>& indices, Vec3 circleCenter, Vec3 endPoint, float radius, const Rgba8& color );

//------------------------------------------------------------------------------------------------------------------------
//												Vertex PCUTBN functions
//------------------------------------------------------------------------------------------------------------------------
void ConvertPCUTBNToPCU( std::vector<Vertex_PCU>& output, std::vector<Vertex_PCUTBN> const& input );
void AppendQuad( std::vector<Vertex_PCUTBN> &out, Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, AABB2 uvs, const Rgba8& color );
void AppendQuad( std::vector<Vertex_PCUTBN> &out, std::vector<uint>& indices, Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, AABB2 uvs, const Rgba8& color );
void AppendUVSphereToIndexedVertexArray( std::vector<Vertex_PCUTBN>& verts, std::vector<uint>& indices, Vec3 center, float radius, uint horizintalCuts, uint verticalCuts, const Rgba8& color );
void AppendIndexedCubeToVerts( std::vector<Vertex_PCUTBN>& verts, std::vector<uint>& indices, const AABB3& aabb3, const Rgba8& color );

void LoadOBJToVertexArray( std::vector<Vertex_PCUTBN>& vertices, char const* filename, mesh_import_options_t const& options );