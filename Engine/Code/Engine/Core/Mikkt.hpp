#pragma once
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include <vector>

void GenerateTangentsForVertexArray( std::vector<Vertex_PCUTBN>& vertices );
void GenerateNormal( Vertex_PCUTBN& vertex0, Vertex_PCUTBN& vertex1, Vertex_PCUTBN& vertex2, bool isClockwise = false );