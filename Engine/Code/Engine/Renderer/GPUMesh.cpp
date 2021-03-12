#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"

GPUMesh::GPUMesh( RenderContext* owner )
	:m_owner(owner)
{
}

GPUMesh::~GPUMesh()
{
	if ( m_vertices )
	{
		delete m_vertices;
		m_vertices = nullptr;
	}
	if( m_indices )
	{
		delete m_indices;
		m_indices = nullptr;
	}
}

void GPUMesh::UpdateVertices( uint vcount, void const* vertexData, uint vertexStride, buffer_attribute_t const* layout )
{
	m_vertexCount = vcount;
	if ( m_vertices )
	{
		delete m_vertices;
		m_vertices = nullptr;
	}

	m_vertices = new VertexBuffer( m_owner, MEMORY_HINT_DYNAMIC );
	size_t bufferTotalByteSize = vcount * vertexStride;
	m_vertices->RenderBuffer::Update( vertexData, bufferTotalByteSize, vertexStride );
	m_vertices->m_element_stride = vertexStride;
	m_vertices->m_layout = layout;
	m_owner->m_currentLayout = layout;
}

void GPUMesh::UpdateIndices( uint icount, uint const* indices )
{
	m_indexCount = icount;
	if ( indices )
	{
		if( m_indices )
		{
			delete m_indices;
			m_indices = nullptr;
		}
		m_indices = new IndexBuffer( m_owner, MEMORY_HINT_DYNAMIC );
		m_indices->IndexBuffer::Update( icount, indices );
	}
}

int GPUMesh::GetIndexCount() const
{
	return m_indexCount;
}

int GPUMesh::GetVertexCount() const
{
	return m_vertexCount;
}
