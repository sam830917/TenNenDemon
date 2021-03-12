#pragma once

struct ID3D11Buffer;
struct buffer_attribute_t;
class RenderContext;

typedef unsigned int uint;
#define BIT_FLAG(b)		(1 << (b))

enum eRenderBufferUsageBit : uint
{
	VERTEX_BUFFER_BIT 		= BIT_FLAG( 0 ),		// A02: can be used to store vertices
	INDEX_BUFFER_BIT		= BIT_FLAG( 1 ),  		// we will discuss this later
	UNIFORM_BUFFER_BIT		= BIT_FLAG( 2 ),		// A03: used to store constants
};
typedef uint eRenderBufferUsage;

enum eRenderMemoryHint
{
	MEMORY_HINT_GPU,		// memory updated and used only from GPU side (if created from CPU, must be initialization data)
	MEMORY_HINT_DYNAMIC, 	// something updated often from CPU side, used by GPU side 
	MEMORY_HINT_STAGING,  // memory that can be read/written from CPU, and copied to a GPU buffer
};

// A GPU Allocator (like malloc on the GPU, new byte[count])
class RenderBuffer
{
public:
	RenderBuffer( RenderContext* owner, eRenderBufferUsage usage, eRenderMemoryHint memoryHint );
	~RenderBuffer();

	bool IsCompatible( size_t dataByteSize, size_t elementByteSize ) const;
	void Cleanup();
	bool Update( void const* data, size_t dataByteSize, size_t elementByteSize );

private:
	bool Create( size_t dataByteSize, size_t elementByteSize );

public:
	RenderContext* m_owner;
	ID3D11Buffer* m_handle;
	eRenderBufferUsage m_usage;
	eRenderMemoryHint m_memHint;

	// stats
	size_t m_bufferByteSize;
	size_t m_elementByteSize; // used for vertex buffer
};

class VertexBuffer : public RenderBuffer
{
public:
	VertexBuffer( RenderContext* ctx, eRenderMemoryHint hint )
		: RenderBuffer( ctx, VERTEX_BUFFER_BIT, hint ) {};

public:
	size_t m_element_stride = 0;
	const buffer_attribute_t*  m_layout;

};