#pragma once
#include "Engine/Renderer/RenderBuffer.hpp"
#include <vector>

class IndexBuffer : public RenderBuffer
{
public:
	IndexBuffer( RenderContext* ctx, eRenderMemoryHint hint )
		: RenderBuffer( ctx, INDEX_BUFFER_BIT, hint ) {};

	void Update( uint icount, uint const* indices );
	void Update( std::vector<uint> const& indices ); // helper, calls one above
};