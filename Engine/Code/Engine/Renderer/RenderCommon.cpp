#include "Engine/Renderer/RenderCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include <cstddef>

buffer_attribute_t::buffer_attribute_t( char const* n, eBufferFormatType t, uint o )
{
	name = n;
	type = t;
	offset = o;
}

buffer_attribute_t::buffer_attribute_t()
{

}