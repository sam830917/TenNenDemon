#include "Engine/Core/Mikkt.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "ThirdParty/MikkT/mikktspace.h"

static int GetNumFaces( SMikkTSpaceContext const* context )
{
	// if you had index buffer
	std::vector<Vertex_PCUTBN>& vertices = *(std::vector<Vertex_PCUTBN>*)(context->m_pUserData);
	return (int) vertices.size() / 3;
}

//-----------------------------------------------------------------
static int GetNumberOfVerticesForFace( SMikkTSpaceContext const* pContext, const int iFace )
{
	UNUSED(pContext);
	UNUSED(iFace);
	return 3;
}

//-----------------------------------------------------------------
static void GetPositionForFaceVert( const SMikkTSpaceContext* pContext, float fvPosOut[], const int iFace, const int iVert )
{
	std::vector<Vertex_PCUTBN>& vertices = *(std::vector<Vertex_PCUTBN>*)(pContext->m_pUserData);
	int indexInVertexArray = iFace * 3 + iVert;

	// get our posistion
	Vec3 ourPos = vertices[indexInVertexArray].m_position;

	fvPosOut[0] = ourPos.x;
	fvPosOut[1] = ourPos.y;
	fvPosOut[2] = ourPos.z;
}

//-----------------------------------------------------------------
static void GetNormalForFaceVert( const SMikkTSpaceContext* pContext, float fvNormOut[], const int iFace, const int iVert )
{
	std::vector<Vertex_PCUTBN>& vertices = *(std::vector<Vertex_PCUTBN>*)(pContext->m_pUserData);
	int indexInVertexArray = iFace * 3 + iVert;

	// get our normal
	Vec3 ourNor = vertices[indexInVertexArray].m_normal;

	fvNormOut[0] = ourNor.x;
	fvNormOut[1] = ourNor.y;
	fvNormOut[2] = ourNor.z;
}

//-----------------------------------------------------------------
static void GetUVForFaceVert( const SMikkTSpaceContext* pContext, float fvTexcOut[], const int iFace, const int iVert )
{
	std::vector<Vertex_PCUTBN>& vertices = *(std::vector<Vertex_PCUTBN>*)(pContext->m_pUserData);
	int indexInVertexArray = iFace * 3 + iVert;

	// get our UV
	Vec2 ourUV = vertices[indexInVertexArray].m_uvTexCoords;

	fvTexcOut[0] = ourUV.x;
	fvTexcOut[1] = ourUV.y;
}

//-----------------------------------------------------------------
static void SetTangent( const SMikkTSpaceContext* pContext,
	const float fvTangent[],
	const float fSign,
	const int iFace, const int iVert )
{
	std::vector<Vertex_PCUTBN>& vertices = *(std::vector<Vertex_PCUTBN>*)(pContext->m_pUserData);
	int indexInVertexArray = iFace * 3 + iVert;

	Vec3 tangent = Vec3( fvTangent[0], fvTangent[1], fvTangent[2] );
	Vec3 normal = vertices[indexInVertexArray].m_normal;
	Vec3 bitangent = CrossProduct3D( normal, tangent ) * fSign;

	vertices[indexInVertexArray].m_tangent = tangent;
	vertices[indexInVertexArray].m_bitangent = bitangent;
}


//-----------------------------------------------------------------
// Assume I'm using only vertex array - not indexed array
void GenerateTangentsForVertexArray( std::vector<Vertex_PCUTBN>& vertices )
{
	SMikkTSpaceInterface interface;

	// How does MikkT get info
	interface.m_getNumFaces = GetNumFaces;
	interface.m_getNumVerticesOfFace = GetNumberOfVerticesForFace;

	interface.m_getPosition = GetPositionForFaceVert;
	interface.m_getNormal = GetNormalForFaceVert;
	interface.m_getTexCoord = GetUVForFaceVert;

	// MikkT telling US info
	interface.m_setTSpaceBasic = SetTangent;
	interface.m_setTSpace = nullptr;

	// Next, the context!  
	// Encapsulate ONE instance of running the algorithm
	SMikkTSpaceContext context;
	context.m_pUserData = &vertices;
	context.m_pInterface = &interface;

	// RUN THE ALGO
	genTangSpaceDefault( &context );
}

void GenerateNormal( Vertex_PCUTBN& vertex0, Vertex_PCUTBN& vertex1, Vertex_PCUTBN& vertex2, bool isClockwise )
{
	Vec3 normal = Vec3::ZERO;

	Vec3 dir1 = vertex1.m_position - vertex0.m_position;
	Vec3 dir2 = vertex2.m_position - vertex0.m_position;

	if ( isClockwise )
	{
		normal = CrossProduct3D( dir2, dir1 );
	}
	else
	{
		normal = CrossProduct3D( dir1, dir2 );
	}
	vertex0.m_normal = normal;
	vertex1.m_normal = normal;
	vertex2.m_normal = normal;
}
