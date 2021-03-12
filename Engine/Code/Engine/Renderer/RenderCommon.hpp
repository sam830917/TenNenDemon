#pragma once
#include <string>
#include <vector>

typedef unsigned int uint;
struct Vertex_PCU;
struct Rgba8;
struct AABB2;
struct AABB3;

enum eBufferFormatType
{
	// add types as you need them - for now, we can get by with just this
	BUFFER_FORMAT_VEC2,
	BUFFER_FORMAT_VEC3,
	BUFFER_FORMAT_R8G8B8A8_UNORM,

	BUFFER_FORMAT_EMPTY
};

struct buffer_attribute_t
{
	std::string name; 			// used to link to a D3D11 shader
								// uint location; 			// used to link to a GL/Vulkan shader 
	eBufferFormatType type = BUFFER_FORMAT_EMPTY;		// what data are we describing
	uint offset; 				// where is it relative to the start of a vertex

	buffer_attribute_t( char const* n, eBufferFormatType t, uint o );
	buffer_attribute_t();
};
