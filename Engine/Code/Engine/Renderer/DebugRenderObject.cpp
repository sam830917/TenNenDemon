#include "Engine/Renderer/DebugRenderObject.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/DebugRender.hpp"

extern BitmapFont* g_theFont;
extern RenderContext* g_currentRenderContext;
extern Clock* g_currentClock;
extern Camera* g_currentCamera;

bool DebugRenderObject::IsOld() const
{
	// Living one frame
	if( m_duration == 0.f )
	{
		return true;
	}
	// Living forever
	if ( m_duration < 0.f )
	{
		return false;
	}

	return m_timer->HasElapsed();
}

DebugRenderObject::~DebugRenderObject()
{
	delete m_timer;
	m_timer = nullptr;
}

Rgba8 DebugRenderObject::GetCurrentColor() const
{
	Rgba8 color = Rgba8::WHITE;
	float remainingTime = (float)m_timer->GetSecondsRemaining();
	remainingTime = RangeMap( 0.f, m_duration, 0.f, 1.f, remainingTime );
	char colorDiff_r = (char)((m_endColor.r - m_startColor.r) * remainingTime);
	char colorDiff_g = (char)((m_endColor.g - m_startColor.g) * remainingTime);
	char colorDiff_b = (char)((m_endColor.b - m_startColor.b) * remainingTime);
	char colorDiff_a = (char)((m_endColor.a - m_startColor.a) * remainingTime);

	color = Rgba8( m_startColor.r + colorDiff_r, m_startColor.g + colorDiff_g, m_startColor.b + colorDiff_b, m_startColor.a +  +colorDiff_a );

	return color;
}

Rgba8 DebugRenderObject::GetCurrentColor( const Rgba8& startColor, const Rgba8& endColor ) const
{
	Rgba8 color = Rgba8::WHITE;
	float remainingTime = (float)m_timer->GetSecondsRemaining();
	remainingTime = RangeMap( 0.f, m_duration, 0.f, 1.f, remainingTime );
	char colorDiff_r = (char)((endColor.r - startColor.r) * remainingTime);
	char colorDiff_g = (char)((endColor.g - startColor.g) * remainingTime);
	char colorDiff_b = (char)((endColor.b - startColor.b) * remainingTime);
	char colorDiff_a = (char)((endColor.a - startColor.a) * remainingTime);

	color = Rgba8( m_startColor.r + colorDiff_r, m_startColor.g + colorDiff_g, m_startColor.b + colorDiff_b, m_startColor.a +  +colorDiff_a );

	return color;
}

//------------------------------------------------------------------------------------------------------------------------
DebugRenderObject_Point::DebugRenderObject_Point( Vec3 pos, float size, Rgba8 start_color, Rgba8 end_color, 
		float duration, eDebugRenderMode mode )
	:m_pos(pos),
	m_size(size)
{
	m_startColor = start_color;
	m_endColor = end_color;
	m_mode = mode;
	m_duration = duration;
	m_timer = new Timer();
	m_timer->SetSeconds( g_currentClock, duration );

	m_mesh = new GPUMesh( g_currentRenderContext );
	std::vector<Vertex_PCU> vertices;
	std::vector<uint> indices;
	AppendUVSphereToIndexedVertexArray( vertices, indices, m_pos, m_size, 20, 20, Rgba8::WHITE );
	m_mesh->UpdateVertices( vertices );
	m_mesh->UpdateIndices( indices );
}

DebugRenderObject_Point::~DebugRenderObject_Point()
{
	delete m_mesh;
	m_mesh = nullptr;
}

void DebugRenderObject_Point::RenderObject() const
{
	Rgba8 color = GetCurrentColor();

	g_currentRenderContext->BindTexture( nullptr );
	g_currentRenderContext->SetModelMatrix( Mat44::IDENTITY );
	g_currentRenderContext->SetModelTint( color );
	g_currentRenderContext->DrawMesh( m_mesh );

	if( DEBUG_RENDER_XRAY == m_mode )
	{
		g_currentRenderContext->EnableDepth( eCompareOp::COMPARISON_GREATER, false );
		Rgba8 xrayColor = Rgba8( (char)(color.r * 0.5f), (char)(color.g * 0.5f), (char)(color.b * 0.5f), color.a );
		g_currentRenderContext->SetModelTint( xrayColor );
		g_currentRenderContext->DrawMesh( m_mesh );
	}
}

//------------------------------------------------------------------------------------------------------------------------
DebugRenderObject_Quad::DebugRenderObject_Quad( Vec3* points, AABB2 uvs, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode )
	:m_uvs(uvs),
	m_point0(points[0]),
	m_point1(points[1]),
	m_point2(points[2]),
	m_point3(points[3])
{
	m_startColor = start_color;
	m_endColor = end_color;
	m_mode = mode;
	m_duration = duration;
	m_timer = new Timer();
	m_timer->SetSeconds( g_currentClock, duration );

	AppendQuad( m_vertices, m_point0, m_point1, m_point2, m_point3, m_uvs, Rgba8::WHITE );
}

DebugRenderObject_Quad::~DebugRenderObject_Quad()
{
}

void DebugRenderObject_Quad::RenderObject() const
{
	Rgba8 color = GetCurrentColor();

	g_currentRenderContext->SetModelTint( color );
	g_currentRenderContext->BindTexture( nullptr );
	g_currentRenderContext->SetModelMatrix( Mat44::IDENTITY );
	g_currentRenderContext->DrawVertexArray( m_vertices );

	if( DEBUG_RENDER_XRAY == m_mode )
	{
		g_currentRenderContext->EnableDepth( eCompareOp::COMPARISON_GREATER, false );
		Rgba8 xrayColor = Rgba8( (char)(color.r * 0.5f), (char)(color.g * 0.5f), (char)(color.b * 0.5f), color.a );
		g_currentRenderContext->SetModelTint( xrayColor );
		g_currentRenderContext->DrawVertexArray( m_vertices );
	}
}

DebugRenderObject_Line::DebugRenderObject_Line( Vec3 p0, Rgba8 p0_start_color, Rgba8 p0_end_color, Vec3 p1, Rgba8 p1_start_color, Rgba8 p1_end_color, float duration, eDebugRenderMode mode )
	:m_pos1(p1),
	m_pos0(p0),
	m_pos1StartColor(p1_start_color),
	m_pos1EndColor(p1_end_color)
{
	m_startColor = p0_start_color;
	m_endColor = p0_end_color;
	m_mode = mode;
	m_duration = duration;
	m_timer = new Timer();
	m_timer->SetSeconds( g_currentClock, duration );
}

DebugRenderObject_Line::~DebugRenderObject_Line()
{
}

void DebugRenderObject_Line::RenderObject() const
{
	Rgba8 startColor = GetCurrentColor( m_startColor, m_endColor );
	Rgba8 endColor = GetCurrentColor( m_pos1StartColor, m_pos1EndColor );

	GPUMesh* mesh = new GPUMesh( g_currentRenderContext );
	std::vector<Vertex_PCU> vertices;
	std::vector<uint> indices;
	AppendCylindarToIndexedVertexArray( vertices, indices, m_pos0, m_pos1, 0.1f, startColor, endColor );
	mesh->UpdateVertices( vertices );
	mesh->UpdateIndices( indices );

	g_currentRenderContext->BindTexture( nullptr );
	g_currentRenderContext->SetModelTint( Rgba8::WHITE );
	g_currentRenderContext->SetModelMatrix( Mat44::IDENTITY );
	g_currentRenderContext->DrawMesh( mesh );

	if( DEBUG_RENDER_XRAY == m_mode )
	{
		g_currentRenderContext->EnableDepth( eCompareOp::COMPARISON_GREATER, false );
		Rgba8 xrayColor = Rgba8( (char)(255.f * 0.5f), (char)(255.f * 0.5f), (char)(255.f * 0.5f), 255 );
		g_currentRenderContext->SetModelTint( xrayColor );
		g_currentRenderContext->DrawMesh( mesh );
	}
	delete mesh;
}

DebugRenderObject_Arrow::DebugRenderObject_Arrow( Vec3 p0, Rgba8 p0_start_color, Rgba8 p0_end_color, Vec3 p1, Rgba8 p1_start_color, Rgba8 p1_end_color, float duration, eDebugRenderMode mode )
	:m_endPoint( p1 ),
	m_startPoint( p0 ),
	m_pos1StartColor( p1_start_color ),
	m_pos1EndColor( p1_end_color )
{
	m_startColor = p0_start_color;
	m_endColor = p0_end_color;
	m_mode = mode;
	m_duration = duration;
	m_timer = new Timer();
	m_timer->SetSeconds( g_currentClock, duration );

	Vec3 translationNor = (m_endPoint - m_startPoint).GetNormalized();
	const float coneLength = 1.f;
	m_coneCenterPoint = m_endPoint - translationNor * coneLength;
}

DebugRenderObject_Arrow::~DebugRenderObject_Arrow()
{
}

void DebugRenderObject_Arrow::RenderObject() const
{
	Rgba8 startColor = GetCurrentColor( m_startColor, m_endColor );
	Rgba8 endColor = GetCurrentColor( m_pos1StartColor, m_pos1EndColor );

	GPUMesh* mesh = new GPUMesh( g_currentRenderContext );
	std::vector<Vertex_PCU> vertices;
	std::vector<uint> indices;
	AppendCylindarToIndexedVertexArray( vertices, indices, m_startPoint, m_coneCenterPoint, 0.1f, startColor, endColor );
	AddConeToIndexedVertexArray( vertices, indices, m_coneCenterPoint, m_endPoint, 0.3f, endColor );
	mesh->UpdateVertices( vertices );
	mesh->UpdateIndices( indices );

	g_currentRenderContext->BindTexture( nullptr );
	g_currentRenderContext->SetModelTint( Rgba8::WHITE );
	g_currentRenderContext->SetModelMatrix( Mat44::IDENTITY );
	g_currentRenderContext->DrawMesh( mesh );

	if( DEBUG_RENDER_XRAY == m_mode )
	{
		g_currentRenderContext->EnableDepth( eCompareOp::COMPARISON_GREATER, false );
		Rgba8 xrayColor = Rgba8( (char)(255.f * 0.5f), (char)(255.f * 0.5f), (char)(255.f * 0.5f), 255 );
		g_currentRenderContext->SetModelTint( xrayColor );
		g_currentRenderContext->DrawMesh( mesh );
	}
	delete mesh;
}

DebugRenderObject_WireBounds::DebugRenderObject_WireBounds( AABB3 bounds, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode )
	:m_bounds(bounds)
{
	m_startColor = start_color;
	m_endColor = end_color;
	m_mode = mode;
	m_duration = duration;
	m_timer = new Timer();
	m_timer->SetSeconds( g_currentClock, duration );

	m_mesh = new GPUMesh( g_currentRenderContext );
	std::vector<Vertex_PCU> vertices;
	std::vector<uint> indices;
	AppendIndexedCubeToVerts( vertices, indices, m_bounds );
	m_mesh->UpdateVertices( vertices );
	m_mesh->UpdateIndices( indices );
}

DebugRenderObject_WireBounds::~DebugRenderObject_WireBounds()
{
	delete m_mesh;
	m_mesh = nullptr;
}

void DebugRenderObject_WireBounds::RenderObject() const
{
	Rgba8 color = GetCurrentColor();

	g_currentRenderContext->SetFillMode( eFillMode::FILL_WIREFRAME );
	g_currentRenderContext->SetCullMode( eCullMode::CULL_NONE );
	g_currentRenderContext->SetModelTint( color );
	g_currentRenderContext->BindTexture( nullptr );
	g_currentRenderContext->SetModelMatrix( Mat44::IDENTITY );
	g_currentRenderContext->DrawMesh( m_mesh );

	if( DEBUG_RENDER_XRAY == m_mode )
	{
		g_currentRenderContext->EnableDepth( eCompareOp::COMPARISON_GREATER, false );
		Rgba8 xrayColor = Rgba8( (char)(color.r * 0.5f), (char)(color.g * 0.5f), (char)(color.b * 0.5f), color.a );
		g_currentRenderContext->SetModelTint( xrayColor );
		g_currentRenderContext->DrawMesh( m_mesh );
	}
	g_currentRenderContext->SetFillMode( eFillMode::FILL_SOLID );
	g_currentRenderContext->SetCullMode( eCullMode::CULL_BACK );
}

DebugRenderObject_WireSphere::DebugRenderObject_WireSphere( Vec3 pos, float size, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode )
	:m_pos( pos ),
	m_size( size )
{
	m_startColor = start_color;
	m_endColor = end_color;
	m_mode = mode;
	m_duration = duration;
	m_timer = new Timer();
	m_timer->SetSeconds( g_currentClock, duration );

	m_mesh = new GPUMesh( g_currentRenderContext );
	std::vector<Vertex_PCU> vertices;
	std::vector<uint> indices;
	AppendUVSphereToIndexedVertexArray( vertices, indices, m_pos, m_size, 20, 20, Rgba8::WHITE );
	m_mesh->UpdateVertices( vertices );
	m_mesh->UpdateIndices( indices );
}

DebugRenderObject_WireSphere::~DebugRenderObject_WireSphere()
{
	delete m_mesh;
	m_mesh = nullptr;
}

void DebugRenderObject_WireSphere::RenderObject() const
{
	Rgba8 color = GetCurrentColor();

	g_currentRenderContext->SetFillMode( eFillMode::FILL_WIREFRAME );
	g_currentRenderContext->SetCullMode( eCullMode::CULL_NONE );
	g_currentRenderContext->BindTexture( nullptr );
	g_currentRenderContext->SetModelMatrix( Mat44::IDENTITY );
	g_currentRenderContext->SetModelTint( color );
	g_currentRenderContext->DrawMesh( m_mesh );

	if( DEBUG_RENDER_XRAY == m_mode )
	{
		g_currentRenderContext->EnableDepth( eCompareOp::COMPARISON_GREATER, false );
		Rgba8 xrayColor = Rgba8( (char)(color.r * 0.5f), (char)(color.g * 0.5f), (char)(color.b * 0.5f), color.a );
		g_currentRenderContext->SetModelTint( xrayColor );
		g_currentRenderContext->DrawMesh( m_mesh );
	}
	g_currentRenderContext->SetFillMode( eFillMode::FILL_SOLID );
	g_currentRenderContext->SetCullMode( eCullMode::CULL_BACK );
}

DebugRenderObject_Basis::DebugRenderObject_Basis( Mat44 basis, Rgba8 start_color, Rgba8 end_color, float duration, float axesRadius, eDebugRenderMode mode )
	:m_basis(basis)
{
	m_startColor = start_color;
	m_endColor = end_color;
	m_mode = mode;
	m_duration = duration;
	m_timer = new Timer();
	m_timer->SetSeconds( g_currentClock, duration );

	m_mesh = new GPUMesh( g_currentRenderContext );
	std::vector<Vertex_PCU> vertices;
	std::vector<uint> indices;
	Vec3 startPoint = Vec3::ZERO;
	const float coneLength = 0.2f;
	Vec3 iBasis = basis.GetIBasis3D();
	Vec3 jBasis = basis.GetJBasis3D();
	Vec3 kBasis = basis.GetKBasis3D();
	float coneRadius = axesRadius * 2.f;

	Vec3 endPoint = Vec3( 1.f, 0.f, 0.f ) * iBasis;
	Vec3 coneCenterPoint = Vec3( 0.8f, 0.f, 0.f ) * iBasis;
	AppendCylindarToIndexedVertexArray( vertices, indices, startPoint, coneCenterPoint, axesRadius, Rgba8::RED );
	AddConeToIndexedVertexArray( vertices, indices, coneCenterPoint, endPoint, coneRadius, Rgba8::RED );

	endPoint = Vec3( 0.f, 1.f, 0.f ) * jBasis;
	coneCenterPoint = Vec3( 0.f, 0.8f, 0.f ) * jBasis;
	AddCylindarToIndexedVertexArray( vertices, indices, startPoint, coneCenterPoint, axesRadius, Rgba8::GREEN, Rgba8::GREEN );
	AddConeToIndexedVertexArray( vertices, indices, coneCenterPoint, endPoint, coneRadius, Rgba8::GREEN );

	endPoint = Vec3( 0.f, 0.f, 1.f ) * kBasis;
	coneCenterPoint = Vec3( 0.f, 0.f, 0.8f ) * kBasis;
	AddCylindarToIndexedVertexArray( vertices, indices, startPoint, coneCenterPoint, axesRadius, Rgba8::BLUE, Rgba8::BLUE );
	AddConeToIndexedVertexArray( vertices, indices, coneCenterPoint, endPoint, coneRadius, Rgba8::BLUE );

	m_mesh->UpdateVertices( vertices );
	m_mesh->UpdateIndices( indices );
}

DebugRenderObject_Basis::~DebugRenderObject_Basis()
{
	delete m_mesh;
	m_mesh = nullptr;
}

void DebugRenderObject_Basis::RenderObject() const
{
	Rgba8 color = GetCurrentColor( m_startColor, m_endColor );

	g_currentRenderContext->BindTexture( nullptr );
	Mat44 m  = Mat44::IDENTITY;
	m.SetTranslation3D( m_basis.GetTranslation3D() );
	g_currentRenderContext->SetModelMatrix( m );
	g_currentRenderContext->SetModelTint( color );
	g_currentRenderContext->DrawMesh( m_mesh );

	if( DEBUG_RENDER_XRAY == m_mode )
	{
		g_currentRenderContext->EnableDepth( eCompareOp::COMPARISON_GREATER, false );
		Rgba8 xrayColor = Rgba8( (char)(255.f * 0.5f), (char)(255.f * 0.5f), (char)(255.f * 0.5f), 255 );
		g_currentRenderContext->SetModelTint( xrayColor );
		g_currentRenderContext->DrawMesh( m_mesh );
	}
}

DebugRenderObject_Text::DebugRenderObject_Text( std::string text, Mat44 basis, Vec3 position, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode )
	:m_basis(basis),
	m_position(position)
{
	m_startColor = start_color;
	m_endColor = end_color;
	m_mode = mode;
	m_duration = duration;
	m_timer = new Timer();
	m_timer->SetSeconds( g_currentClock, duration );

	AppendText3D( m_vertices, text, position, 1.f, g_theFont, Rgba8::WHITE );
}

DebugRenderObject_Text::~DebugRenderObject_Text()
{
}

void DebugRenderObject_Text::RenderObject() const
{
	Rgba8 color = GetCurrentColor();

	g_currentRenderContext->SetModelTint( color );
	g_currentRenderContext->BindTexture( g_theFont->GetTexture() );
	g_currentRenderContext->SetModelMatrix( m_basis );
	g_currentRenderContext->DrawVertexArray( m_vertices );

	if( DEBUG_RENDER_XRAY == m_mode )
	{
		g_currentRenderContext->EnableDepth( eCompareOp::COMPARISON_GREATER, false );
		Rgba8 xrayColor = Rgba8( (char)(color.r * 0.5f), (char)(color.g * 0.5f), (char)(color.b * 0.5f), color.a );
		g_currentRenderContext->SetModelTint( xrayColor );
		g_currentRenderContext->DrawVertexArray( m_vertices );
	}
}

DebugRenderObject_BillboardText::DebugRenderObject_BillboardText( std::string text, Vec3 position, Vec2 pivot, Rgba8 start_color, Rgba8 end_color, 
	float duration, eDebugRenderMode mode )
	:m_pivot( pivot ),
	m_worldPosition( position )
{
	m_startColor = start_color;
	m_endColor = end_color;
	m_mode = mode;
	m_duration = duration;
	m_timer = new Timer();
	m_timer->SetSeconds( g_currentClock, duration );

	Vec2 textDimension = g_theFont->GetDimensionsForText2D( 1.f, text );
	Vec3 localPosition = Vec3( -textDimension.x * pivot.x, -textDimension.y * pivot.y, 0.f );

	AppendText3D( m_vertices, text, localPosition, 1.f, g_theFont, Rgba8::WHITE );
}

DebugRenderObject_BillboardText::~DebugRenderObject_BillboardText()
{
}

void DebugRenderObject_BillboardText::RenderObject() const
{
	Rgba8 color = GetCurrentColor();

	Mat44 m = g_currentCamera->m_transform.GetMatrix();
	m.SetTranslation3D(m_worldPosition);
	g_currentRenderContext->SetModelTint( color );
	g_currentRenderContext->BindTexture( g_theFont->GetTexture() );
	g_currentRenderContext->SetModelMatrix( m );
	g_currentRenderContext->DrawVertexArray( m_vertices );

	if( DEBUG_RENDER_XRAY == m_mode )
	{
		g_currentRenderContext->EnableDepth( eCompareOp::COMPARISON_GREATER, false );
		Rgba8 xrayColor = Rgba8( (char)(color.r * 0.5f), (char)(color.g * 0.5f), (char)(color.b * 0.5f), color.a );
		g_currentRenderContext->SetModelTint( xrayColor );
		g_currentRenderContext->DrawVertexArray( m_vertices );
	}
}


DebugRenderObject_ScreenPoint::DebugRenderObject_ScreenPoint( Vec2 center, float size, Rgba8 start_color, Rgba8 end_color, float duration )
	:m_center(center),
	m_size(size)
{
	m_startColor = start_color;
	m_endColor = end_color;
	m_duration = duration;
	m_timer = new Timer();
	m_timer->SetSeconds( g_currentClock, duration );

	AppendCircle( m_vertices, Vec3( center, 0.f ), m_size, Rgba8::WHITE );
}

DebugRenderObject_ScreenPoint::~DebugRenderObject_ScreenPoint()
{
}

void DebugRenderObject_ScreenPoint::RenderObject() const
{
	Rgba8 color = GetCurrentColor();
	g_currentRenderContext->BindTexture( nullptr );
	g_currentRenderContext->SetModelTint( color );
	g_currentRenderContext->DrawVertexArray( m_vertices );
}

DebugRenderObject_ScreenLine::DebugRenderObject_ScreenLine( Vec2 p0, Rgba8 p0_start_color, Rgba8 p0_end_color, 
	Vec2 p1, Rgba8 p1_start_color, Rgba8 p1_end_color, float duration )
{
	m_pos0 = p0;
	m_startColor = p0_start_color;
	m_endColor = p0_end_color;
	m_pos1 = p1;
	m_pos1StartColor = p1_start_color;
	m_pos1EndColor = p1_end_color;

	m_duration = duration;
	m_timer = new Timer();
	m_timer->SetSeconds( g_currentClock, duration );

	Vec2 center = (p1 + p0) * 0.5f;
	float width = (p1 - p0).GetLength();
	OBB2 obb = OBB2( center, Vec2( width, 10.f ), Vec2( 1.f, 0.f ) );
	obb.SetOrientationDegrees( (p1 - p0).GetAngleDegrees() );
	m_line = obb;
}

DebugRenderObject_ScreenLine::~DebugRenderObject_ScreenLine()
{
}

void DebugRenderObject_ScreenLine::RenderObject() const
{
	Rgba8 startColor = GetCurrentColor( m_startColor, m_endColor );
	Rgba8 endColor = GetCurrentColor( m_pos1StartColor, m_pos1EndColor );
	std::vector<Vertex_PCU> vertices;
	Vec2 point[4];
	m_line.GetCornerPosition( point );
	AppendQuad( vertices, Vec3(point[0], 0.f), Vec3(point[1], 0.f), Vec3(point[2], 0.f), Vec3(point[3], 0.f), AABB2::ZERO_TO_ONE, startColor, endColor, startColor, endColor );

	g_currentRenderContext->BindTexture( nullptr );
	g_currentRenderContext->SetModelTint( Rgba8::WHITE );
	g_currentRenderContext->DrawVertexArray( vertices );
}

DebugRenderObject_ScreenArrow::DebugRenderObject_ScreenArrow( Vec2 p0, Rgba8 p0_start_color, Rgba8 p0_end_color, Vec2 p1, Rgba8 p1_start_color, Rgba8 p1_end_color, float duration )
{
	m_startPos = p0;
	m_startColor = p0_start_color;
	m_endColor = p0_end_color;
	m_endPos = p1;
	m_pos1StartColor = p1_start_color;
	m_pos1EndColor = p1_end_color;

	m_duration = duration;
	m_timer = new Timer();
	m_timer->SetSeconds( g_currentClock, duration );

	float triHeight = 40.f;
	Vec2 lineEndPos = p1 - p0;
	lineEndPos.SetLength( lineEndPos.GetLength() - triHeight );
	Vec2 center = (lineEndPos + p0) * 0.5f;
	Vec2 translation = lineEndPos - p0;
	float width = (lineEndPos - p0).GetLength();
	OBB2 obb = OBB2( center, Vec2( width, 10.f ), Vec2( 1.f, 0.f ) );
	obb.SetOrientationDegrees( translation.GetAngleDegrees() );
	m_line = obb;

	Vec2 fullTranslation = p1 - p0;
	fullTranslation.SetLength( fullTranslation.GetLength() - triHeight );
	fullTranslation.RotateDegrees( 5.f );
	m_arrowPos0 = fullTranslation;
	fullTranslation.RotateDegrees( -10.f );
	m_arrowPos1 = fullTranslation;
}

DebugRenderObject_ScreenArrow::~DebugRenderObject_ScreenArrow()
{
}

void DebugRenderObject_ScreenArrow::RenderObject() const
{
	Rgba8 startColor = GetCurrentColor( m_startColor, m_endColor );
	Rgba8 endColor = GetCurrentColor( m_pos1StartColor, m_pos1EndColor );
	std::vector<Vertex_PCU> vertices;
	Vec2 point[4];
	m_line.GetCornerPosition( point );
	AppendQuad( vertices, Vec3( point[0], 0.f ), Vec3( point[1], 0.f ), Vec3( point[2], 0.f ), Vec3( point[3], 0.f ), AABB2::ZERO_TO_ONE, startColor, endColor, startColor, endColor );
	AppendTriangle2D( vertices, m_endPos, m_arrowPos0, m_arrowPos1, endColor );

	g_currentRenderContext->BindTexture( nullptr );
	g_currentRenderContext->SetModelTint( Rgba8::WHITE );
	g_currentRenderContext->DrawVertexArray( vertices );
}

DebugRenderObject_ScreenQuad::DebugRenderObject_ScreenQuad( AABB2 bounds, Rgba8 start_color, Rgba8 end_color, float duration, AABB2 uvs, const Texture* tex )
{
	m_startColor = start_color;
	m_endColor = end_color;
	m_duration = duration;
	m_timer = new Timer();
	m_timer->SetSeconds( g_currentClock, duration );
	m_bounds = bounds;
	m_tex = tex;
	m_uvs = uvs;

	Vec3 pos0 = Vec3( bounds.mins, 0.f );
	Vec3 pos1 = Vec3( Vec2( bounds.maxs.x, bounds.mins.y ), 0.f );
	Vec3 pos2 = Vec3( Vec2( bounds.mins.x, bounds.maxs.y ), 0.f );
	Vec3 pos3 = Vec3( bounds.maxs, 0.f );
	AppendQuad( m_vertices, pos0, pos1, pos2, pos3, uvs, Rgba8::WHITE );
}

DebugRenderObject_ScreenQuad::~DebugRenderObject_ScreenQuad()
{
}

void DebugRenderObject_ScreenQuad::RenderObject() const
{
	Rgba8 color = GetCurrentColor();
	g_currentRenderContext->BindTexture( m_tex );
	g_currentRenderContext->SetModelTint( color );
	g_currentRenderContext->DrawVertexArray( m_vertices );
}

DebugRenderObject_ScreenText::DebugRenderObject_ScreenText( std::string text, Vec2 pos, float size, Rgba8 start_color, Rgba8 end_color, float duration )
{
	m_startColor = start_color;
	m_endColor = end_color;
	m_duration = duration;
	m_timer = new Timer();
	m_timer->SetSeconds( g_currentClock, duration );
	m_position = pos;
	m_size = size;

	AppendText3D( m_vertices, text, Vec3( pos, 0.f ), size, g_theFont, Rgba8::WHITE );
}

DebugRenderObject_ScreenText::~DebugRenderObject_ScreenText()
{
}

void DebugRenderObject_ScreenText::RenderObject() const
{
	Rgba8 color = GetCurrentColor();
	g_currentRenderContext->BindTexture( g_theFont->GetTexture() );
	g_currentRenderContext->SetModelTint( color );
	g_currentRenderContext->DrawVertexArray( m_vertices );
}

DebugRenderObject_ScreenMessage::DebugRenderObject_ScreenMessage( std::string text, Rgba8 start_color, Rgba8 end_color, float duration )
{
	m_startColor = start_color;
	m_endColor = end_color;
	m_duration = duration;
	m_timer = new Timer();
	m_timer->SetSeconds( g_currentClock, duration );
	m_text = text;
}

DebugRenderObject_ScreenMessage::~DebugRenderObject_ScreenMessage()
{
}

void DebugRenderObject_ScreenMessage::RenderObject() const
{
	RenderMessage( 0 );
}

void DebugRenderObject_ScreenMessage::RenderMessage( uint index ) const
{
	Rgba8 color = GetCurrentColor();
	const float lineHegiht = 30.f;

	AABB2 screenBox = DebugGetScreenBounds();
	Vec2 textDimension = g_theFont->GetDimensionsForText2D(lineHegiht, m_text);

	Vec2 printPos = Vec2( screenBox.maxs.x - textDimension.x, screenBox.maxs.y - textDimension.y - ( textDimension.y * index ) );

	std::vector<Vertex_PCU> textVerts;
	g_theFont->AddVertsForText2D( textVerts, printPos, lineHegiht, m_text );
	g_currentRenderContext->BindTexture( g_theFont->GetTexture() );
	g_currentRenderContext->SetModelTint( color );
	g_currentRenderContext->DrawVertexArray(textVerts);
}