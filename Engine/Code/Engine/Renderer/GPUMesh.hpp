#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Renderer/RenderCommon.hpp"

class RenderContext;
class IndexBuffer;
class VertexBuffer;

class GPUMesh
{
public:
	GPUMesh( RenderContext* owner );
	~GPUMesh();

	void UpdateVertices( uint vcount, void const* vertexData, uint vertexStride, buffer_attribute_t const* layout );
	void UpdateIndices( uint icount, uint const* indices );

	int GetIndexCount() const;
	int GetVertexCount() const;
	VertexBuffer* GetVertexBuffer() const { return m_vertices; }
	IndexBuffer* GetIndexBuffer() const { return m_indices; }

	void UpdateIndices( std::vector<uint> const& indices )
	{
		UpdateIndices( (uint) indices.size(), &indices[0] );
	}

	void UpdateVertices( std::vector<Vertex_PCU> const& vertices )
	{
		UpdateVertices( (uint) vertices.size(),
			&vertices[0],
			sizeof( Vertex_PCU ),
			Vertex_PCU::LAYOUT );
	}

	void UpdateVertices( std::vector<Vertex_PCUTBN> const& vertices )
	{
		UpdateVertices( (uint)vertices.size(),
			&vertices[0],
			sizeof( Vertex_PCUTBN ),
			Vertex_PCUTBN::LAYOUT );
	}

	// helper template
	template <typename VERTEX_TYPE>
	void UpdateVertices( uint vcount, VERTEX_TYPE const* vertices )
	{
		UpdateVertices( vcount, vertices, sizeof( VERTEX_TYPE ), VERTEX_TYPE::LAYOUT );
	}

public:
	RenderContext* m_owner = nullptr;

	uint m_indexCount = 0;
	uint m_vertexCount = 0;
	VertexBuffer* m_vertices = nullptr;  // like a std::vector<VertexPCU> -> but on GPU
	IndexBuffer* m_indices = nullptr; 	// like a std::vector<uint> -> but on the GPU

	// optionally
	// std::vector<uint8_t> m_cpuVertexData; 
	// std::Vector<uint> m_cpuIndexData; 
};