#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/RenderCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"

RenderBuffer::RenderBuffer( RenderContext* owner, eRenderBufferUsage usage, eRenderMemoryHint memoryHint )
{
	m_owner = owner;
	m_usage = usage;
	m_memHint = memoryHint;

	m_handle = nullptr;
	m_bufferByteSize = 0U;
	m_elementByteSize = 0U;
}

RenderBuffer::~RenderBuffer()
{
	DX_SAFE_RELEASE(m_handle);
}

bool RenderBuffer::IsCompatible( size_t dataByteSize, size_t elementByteSize ) const
{
	if (m_handle == nullptr)
	{
		return false;
	}

	if (m_elementByteSize != elementByteSize)
	{
		return false;
	}

	if (m_memHint == MEMORY_HINT_DYNAMIC)
	{
		return dataByteSize <= m_bufferByteSize;
	}
	else
	{
		return dataByteSize == m_bufferByteSize;
	}
}

void RenderBuffer::Cleanup()
{
	DX_SAFE_RELEASE(m_handle);
	m_bufferByteSize = 0;
	m_elementByteSize = 0;
}

bool RenderBuffer::Update( void const* data, size_t dataByteSize, size_t elementByteSize )
{
	// 1. if not compatible - destroy the old buffer
	// our elementSize matches the passed in
	// if we're GPU
		// bufferSizes MUST match
	// if we're dynamic
		// passed in buffer size is less than our bufferSize

	// 2. if not buffer, create one that is compatible
	if ( !IsCompatible( dataByteSize, elementByteSize ) )
	{
		Cleanup();
		Create( dataByteSize, elementByteSize );
	}

	// 3. updating the buffer
	ID3D11DeviceContext* ctx = m_owner->m_context;
	if ( m_memHint == MEMORY_HINT_DYNAMIC )
	{
		// Mapping
		D3D11_MAPPED_SUBRESOURCE mapped;

		// CPU -> GPU memory copy
		HRESULT result = ctx->Map( m_handle, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped );
		if ( SUCCEEDED(result) )
		{
			memcpy( mapped.pData, data, dataByteSize );
			ctx->Unmap( m_handle, 0 );
		}
		else
		{
			return false;
		}
		// Only available to DYNAMIC buffer
		// But, don't have to reallocate if going smaller
	}
	else
	{
		// if this is MEMORY_HINT_GPU
		ctx->UpdateSubresource( m_handle, 0, nullptr, data, 0, 0 );
	}
	// CopySubresource (direct copy)
	// This is only available to GPU buffers that have exactly
	// the same size, and element size

	return true;
}

UINT ToDXUsage( eRenderBufferUsage usage )
{
	UINT ret = 0;

	if (usage & VERTEX_BUFFER_BIT)
	{
		ret |= D3D11_BIND_VERTEX_BUFFER;
	}

	if (usage & INDEX_BUFFER_BIT)
	{
		ret |= D3D11_BIND_INDEX_BUFFER;
	}

	if( usage & UNIFORM_BUFFER_BIT )
	{
		ret |= D3D11_BIND_CONSTANT_BUFFER;
	}

	return ret;
}

D3D11_USAGE ToDXMemoryUsage( eRenderMemoryHint hint )
{
	switch( hint )
	{
	case MEMORY_HINT_GPU:		return D3D11_USAGE_DEFAULT;
	case MEMORY_HINT_DYNAMIC:	return D3D11_USAGE_DYNAMIC;
	case MEMORY_HINT_STAGING:	return D3D11_USAGE_STAGING;
	default:					ERROR_AND_DIE( "Unknown hint" );
	}
}

bool RenderBuffer::Create( size_t dataByteSize, size_t elementByteSize )
{
	ID3D11Device* device = m_owner->m_device;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = (UINT)dataByteSize;
	desc.Usage = ToDXMemoryUsage( m_memHint );
	desc.BindFlags = ToDXUsage( m_usage );
	desc.CPUAccessFlags = 0;
	if (m_memHint == MEMORY_HINT_DYNAMIC)
	{
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (m_memHint == MEMORY_HINT_STAGING)
	{
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}
	desc.MiscFlags = 0; // used in special cases
	desc.StructureByteStride = (UINT)elementByteSize;
	m_elementByteSize = elementByteSize;
	m_bufferByteSize = dataByteSize;

	device->CreateBuffer( &desc, nullptr, &m_handle );
	return (m_handle != nullptr);
}
