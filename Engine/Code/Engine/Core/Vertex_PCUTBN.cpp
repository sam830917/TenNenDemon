#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Renderer/RenderCommon.hpp"

Vertex_PCUTBN::Vertex_PCUTBN()
{
}

Vertex_PCUTBN::~Vertex_PCUTBN()
{
}

Vertex_PCUTBN::Vertex_PCUTBN( const Vec3& position, const Rgba8& tint, const Vec2& uvTexCoords )
	: m_color( tint ), 
	m_uvTexCoords( uvTexCoords ), 
	m_position( position )
{
}

Vertex_PCUTBN::Vertex_PCUTBN( const Vec3& position, const Rgba8& tint, const Vec2& uvTexCoords, const Vec3& tangent, const Vec3& bitangent, const Vec3& normal )
	: m_color( tint ), 
	m_uvTexCoords( uvTexCoords ), 
	m_position( position ),
	m_tangent( tangent ),
	m_bitangent( bitangent ),
	m_normal( normal )
{
}

buffer_attribute_t const Vertex_PCUTBN::LAYOUT[] =
{
	buffer_attribute_t( "POSITION",  BUFFER_FORMAT_VEC3,      		offsetof( Vertex_PCUTBN, m_position ) ),
	buffer_attribute_t( "COLOR",     BUFFER_FORMAT_R8G8B8A8_UNORM, 	offsetof( Vertex_PCUTBN, m_color ) ),
	buffer_attribute_t( "TEXCOORD",	 BUFFER_FORMAT_VEC2,			offsetof( Vertex_PCUTBN, m_uvTexCoords ) ),
	buffer_attribute_t( "TANGENT",	 BUFFER_FORMAT_VEC3,			offsetof( Vertex_PCUTBN, m_tangent ) ),
	buffer_attribute_t( "BITANGENT", BUFFER_FORMAT_VEC3,			offsetof( Vertex_PCUTBN, m_bitangent ) ),
	buffer_attribute_t( "NORMAL",	 BUFFER_FORMAT_VEC3,			offsetof( Vertex_PCUTBN, m_normal ) ),
	buffer_attribute_t() // end - terminator element; 
};