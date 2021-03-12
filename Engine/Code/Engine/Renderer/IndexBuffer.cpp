#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"

void IndexBuffer::Update( uint icount, uint const* indices )
{
	size_t elementByteSize = sizeof( uint );
	size_t bufferTotalByteSize = icount * elementByteSize;
	RenderBuffer::Update( indices, bufferTotalByteSize, elementByteSize );
}

void IndexBuffer::Update( std::vector<uint> const& indices )
{
	Update( (int) indices.size(), &indices[0] );
}
