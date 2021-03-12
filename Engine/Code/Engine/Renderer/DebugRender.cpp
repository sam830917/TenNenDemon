#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/DebugRenderObject.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"

extern BitmapFont* g_theFont;
extern RenderContext* g_theRenderer;

DebugRenderSystem* g_debugRenderSystem = nullptr;
RenderContext* g_currentRenderContext = nullptr;
Clock* g_currentClock = nullptr;
Camera* g_currentCamera = nullptr;

class DebugRenderSystem
{
public:
	void AddWorldObject( DebugRenderObject* obj );
	void AddScreenObject( DebugRenderObject* obj );
	void DeleteWorldObject( uint index );
	void DeleteScreenObject( uint index );

public:
	bool isRendering = true;

	RenderContext* m_context;
	Camera* m_camera;
	Clock* m_clock;

	float m_screenHeight = 0.f;
	float m_screenWidth = 0.f;

	std::vector<DebugRenderObject*> m_worldObjectList;
	std::vector<DebugRenderObject*> m_screenObjectList;
	std::vector<DebugRenderObject_ScreenMessage*> m_screenMessageObjectList;
};

void DebugRenderSystemStartup()
{
	g_debugRenderSystem = new DebugRenderSystem();
	g_debugRenderSystem->m_clock = new Clock();
	g_currentClock = g_debugRenderSystem->m_clock;
	g_currentRenderContext = g_theRenderer;
}

void DebugRenderSystemShutdown()
{
	for( uint i = 0; i < (uint)g_debugRenderSystem->m_worldObjectList.size(); ++i )
	{
		DebugRenderObject* obj = g_debugRenderSystem->m_worldObjectList[i];
		if( obj )
		{
			g_debugRenderSystem->DeleteWorldObject( i );
			--i;
		}
	}
	for( uint i = 0; i < (uint)g_debugRenderSystem->m_screenObjectList.size(); ++i )
	{
		DebugRenderObject* obj = g_debugRenderSystem->m_screenObjectList[i];
		if( obj )
		{
			g_debugRenderSystem->DeleteScreenObject( i );
			--i;
		}
	}
	for( uint i = 0; i < (uint)g_debugRenderSystem->m_screenMessageObjectList.size(); ++i )
	{
		DebugRenderObject* obj = g_debugRenderSystem->m_screenMessageObjectList[i];
		if( obj )
		{
			delete g_debugRenderSystem->m_screenMessageObjectList[i];
			g_debugRenderSystem->m_screenMessageObjectList[i] = nullptr;

			uint lastIndex = (uint)g_debugRenderSystem->m_screenMessageObjectList.size() - 1;
			g_debugRenderSystem->m_screenMessageObjectList[i] = g_debugRenderSystem->m_screenMessageObjectList[lastIndex];
			g_debugRenderSystem->m_screenMessageObjectList.pop_back();
			--i;
		}
	}

	delete g_debugRenderSystem;
	g_debugRenderSystem = nullptr;
}

void EnableDebugRendering()
{
	g_debugRenderSystem->isRendering = true;
}

void DisableDebugRendering()
{
	g_debugRenderSystem->isRendering = false;
}

void ClearDebugRendering()
{
	for( uint i = 0; i < (uint)g_debugRenderSystem->m_worldObjectList.size(); ++i )
	{
		DebugRenderObject* obj = g_debugRenderSystem->m_worldObjectList[i];
		if( obj )
		{
			g_debugRenderSystem->DeleteWorldObject( i );
			--i;
		}
	}
	for( uint i = 0; i < (uint)g_debugRenderSystem->m_screenObjectList.size(); ++i )
	{
		DebugRenderObject* obj = g_debugRenderSystem->m_screenObjectList[i];
		if( obj )
		{
			g_debugRenderSystem->DeleteScreenObject( i );
			--i;
		}
	}
	for( uint i = 0; i < (uint)g_debugRenderSystem->m_screenMessageObjectList.size(); ++i )
	{
		DebugRenderObject* obj = g_debugRenderSystem->m_screenMessageObjectList[i];
		if( obj )
		{
			delete g_debugRenderSystem->m_screenMessageObjectList[i];
			g_debugRenderSystem->m_screenMessageObjectList[i] = nullptr;

			uint lastIndex = (uint)g_debugRenderSystem->m_screenMessageObjectList.size() - 1;
			g_debugRenderSystem->m_screenMessageObjectList[i] = g_debugRenderSystem->m_screenMessageObjectList[lastIndex];
			g_debugRenderSystem->m_screenMessageObjectList.pop_back();
			--i;
		}
	}
}

void DebugRenderBeginFrame()
{
	// Does nothing, here for completeness.
}

void DebugRenderWorldToCamera( Camera* cam )
{
	g_debugRenderSystem->m_camera = cam;
	g_debugRenderSystem->m_context = cam->m_owner;
	g_currentRenderContext = cam->m_owner;
	g_currentCamera = cam;

	if ( !g_debugRenderSystem->isRendering )
		return;

	const uint objSize = (uint) g_debugRenderSystem->m_worldObjectList.size();
	for ( uint i = 0; i < objSize; ++i )
	{
		DebugRenderObject* obj = g_debugRenderSystem->m_worldObjectList[i];

		if ( obj )
		{
			if( DEBUG_RENDER_ALWAYS == obj->m_mode )
			{
				g_debugRenderSystem->m_context->DisableDepth();
			}
			else if( DEBUG_RENDER_XRAY == obj->m_mode )
			{
				g_debugRenderSystem->m_context->EnableDepth( eCompareOp::COMPARISON_LESS_EQUAL, false );
			}
			else
			{
				g_debugRenderSystem->m_context->EnableDepth( eCompareOp::COMPARISON_LESS_EQUAL, true );
			}

			obj->RenderObject();
		}
	}
}

void DebugRenderScreenTo( RenderContext* context )
{
	RenderContext* ctx = context;
	g_debugRenderSystem->m_context = ctx;
	g_currentRenderContext = ctx;

	if( !g_debugRenderSystem->isRendering )
		return;

	Camera* camera = new Camera();
	Vec2 min = Vec2::ZERO;
	Vec2 max = Vec2( g_debugRenderSystem->m_screenWidth, g_debugRenderSystem->m_screenHeight );
	camera->SetProjectionOrthographic( min, max, -1.0f, 1.0f );
	camera->InitialUBO( ctx );
	camera->SetClearMode( CLEAR_NONE );
	g_debugRenderSystem->m_camera = camera;
	g_currentCamera = camera;

	ctx->BeginCamera( *camera );

	uint objSize = (uint)g_debugRenderSystem->m_screenObjectList.size();
	for( uint i = 0; i < objSize; ++i )
	{
		DebugRenderObject* obj = g_debugRenderSystem->m_screenObjectList[i];

		if( obj )
		{
			obj->RenderObject();
		}
	}
	
	objSize = (uint)g_debugRenderSystem->m_screenMessageObjectList.size();
	uint idx = 0;
	for( uint i = 0; i < objSize; ++i )
	{
		DebugRenderObject_ScreenMessage* obj = g_debugRenderSystem->m_screenMessageObjectList[i];

		if( obj )
		{
			obj->RenderMessage(idx++);
		}
	}

	ctx->EndCamera( *camera );

	camera->ClearupUBO();
	delete camera;
}

void DebugRenderEndFrame()
{
	// foreach object, if ready to culled, remove and delete
	for ( uint i = 0; i < (uint) g_debugRenderSystem->m_worldObjectList.size(); ++i )
	{
		DebugRenderObject* obj = g_debugRenderSystem->m_worldObjectList[i];
		if ( obj && obj->IsOld() )
		{
			g_debugRenderSystem->DeleteWorldObject(i);
			--i;
		}
	}
	for( uint i = 0; i < (uint) g_debugRenderSystem->m_screenObjectList.size(); ++i )
	{
		DebugRenderObject* obj = g_debugRenderSystem->m_screenObjectList[i];
		if( obj && obj->IsOld() )
		{
			g_debugRenderSystem->DeleteScreenObject( i );
			--i;
		}
	}
	for( uint i = 0; i < (uint) g_debugRenderSystem->m_screenMessageObjectList.size(); ++i )
	{
		DebugRenderObject* obj = g_debugRenderSystem->m_screenMessageObjectList[i];
		if( obj && obj->IsOld() )
		{
			delete g_debugRenderSystem->m_screenMessageObjectList[i];
			g_debugRenderSystem->m_screenMessageObjectList[i] = nullptr;

			uint lastIndex = (uint)g_debugRenderSystem->m_screenMessageObjectList.size() - 1;
			g_debugRenderSystem->m_screenMessageObjectList[i] = g_debugRenderSystem->m_screenMessageObjectList[lastIndex];
			g_debugRenderSystem->m_screenMessageObjectList.pop_back();
			--i;
		}
	}
}

void DebugAddWorldPoint( Vec3 pos, float size, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode )
{
	DebugRenderObject_Point* pointObj = new DebugRenderObject_Point( pos, size, start_color, end_color, duration, mode );

	g_debugRenderSystem->AddWorldObject( pointObj );
}

void DebugAddWorldPoint( Vec3 pos, float size, Rgba8 color, float duration, eDebugRenderMode mode )
{
	DebugAddWorldPoint( pos, size, color, color, duration, mode );
}

void DebugAddWorldPoint( Vec3 pos, Rgba8 color, float duration, eDebugRenderMode mode )
{
	DebugAddWorldPoint( pos, 1.f, color, color, duration, mode );
}

void DebugAddWorldLine( Vec3 p0, Rgba8 p0_start_color, Rgba8 p0_end_color, Vec3 p1, Rgba8 p1_start_color, Rgba8 p1_end_color, float duration, eDebugRenderMode mode )
{
	DebugRenderObject_Line* lineObj = new DebugRenderObject_Line( p0, p0_start_color, p0_end_color, p1, p1_start_color, p1_end_color,
		duration, mode );

	g_debugRenderSystem->AddWorldObject( lineObj );
}

void DebugAddWorldLine( Vec3 start, Vec3 end, Rgba8 color, float duration, eDebugRenderMode mode )
{
	DebugAddWorldLine( start, color, color, end, color, color, duration, mode );
}

void DebugAddWorldLineStrip( uint count, Vec3 const* positions, Rgba8 start_p0_color, Rgba8 start_pf_color, Rgba8 end_p0_color, Rgba8 end_pf_color, float duration, eDebugRenderMode mode )
{
	UNUSED(count);
	UNUSED(positions);
	UNUSED(start_p0_color);
	UNUSED(start_pf_color);
	UNUSED(end_p0_color);
	UNUSED(end_pf_color);
	UNUSED(duration);
	UNUSED(mode);
}

void DebugAddWorldArrow( Vec3 p0, Rgba8 p0_start_color, Rgba8 p0_end_color, Vec3 p1, Rgba8 p1_start_color, Rgba8 p1_end_color, float duration, eDebugRenderMode mode )
{
	DebugRenderObject_Arrow* arrowObj = new DebugRenderObject_Arrow( p0, p0_start_color, p0_end_color, p1, p1_start_color, p1_end_color,
		duration, mode );

	g_debugRenderSystem->AddWorldObject( arrowObj );
}

void DebugAddWorldArrow( Vec3 start, Vec3 end, Rgba8 color, float duration, eDebugRenderMode mode )
{
	DebugAddWorldArrow( start, color, color, end, color, color, duration, mode );
}

void DebugAddWorldQuad( Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, AABB2 uvs, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode )
{
	// 2 --- 3
	// | \   |
	// |   \ |
	// 0 --- 1
	Vec3 points[4] = { p0, p1, p2, p3 };
	DebugRenderObject_Quad* quadObj = new DebugRenderObject_Quad( points, uvs, start_color, end_color, duration, mode );

	g_debugRenderSystem->AddWorldObject( quadObj );
}

void DebugAddWorldWireBounds( AABB3 bounds, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode )
{
	DebugRenderObject_WireBounds* boxObj = new DebugRenderObject_WireBounds( bounds, start_color, end_color, duration, mode );

	g_debugRenderSystem->AddWorldObject( boxObj );
}

void DebugAddWorldWireBounds( AABB3 bounds, Rgba8 color, float duration, eDebugRenderMode mode )
{
	DebugAddWorldWireBounds( bounds, color, color, duration, mode );
}

void DebugAddWorldWireSphere( Vec3 pos, float radius, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode )
{
	DebugRenderObject_WireSphere* sphereObj = new DebugRenderObject_WireSphere( pos, radius, start_color, end_color, duration, mode );

	g_debugRenderSystem->AddWorldObject( sphereObj );
}

void DebugAddWorldWireSphere( Vec3 pos, float radius, Rgba8 color, float duration, eDebugRenderMode mode )
{
	DebugAddWorldWireSphere( pos, radius, color, color, duration, mode );
}

void DebugAddWorldBasis( Mat44 basis, Rgba8 start_tint, Rgba8 end_tint, float duration, float axesRadius, eDebugRenderMode mode )
{
	DebugRenderObject_Basis* arrowObj = new DebugRenderObject_Basis( basis, start_tint, end_tint, duration, axesRadius, mode );

	g_debugRenderSystem->AddWorldObject( arrowObj );
}

void DebugAddWorldBasis( Mat44 basis, Rgba8 start_tint, Rgba8 end_tint, float duration, eDebugRenderMode mode )
{
	DebugAddWorldBasis( basis, Rgba8::WHITE, Rgba8::WHITE, duration, 0.05f, mode );
}

void DebugAddWorldBasis( Mat44 basis, float duration, eDebugRenderMode mode )
{
	DebugAddWorldBasis( basis, Rgba8::WHITE, Rgba8::WHITE, duration, 0.05f, mode );
}

void DebugAddWorldText( Mat44 basis, Vec2 pivot, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode, char const* text )
{
	Vec2 textDimension = g_theFont->GetDimensionsForText2D( 1.f, text );
	Vec3 position = Vec3( -textDimension.x * pivot.x, -textDimension.y * pivot.y, 0.f );

	DebugRenderObject_Text* textObj = new DebugRenderObject_Text( text, basis, position, start_color, end_color, duration, mode );

	g_debugRenderSystem->AddWorldObject( textObj );
}

void DebugAddWorldTextf( Mat44 basis, Vec2 pivot, Rgba8 color, float duration, eDebugRenderMode mode, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string str = Stringv( format, args );

	DebugAddWorldText( basis, pivot, color, color, duration, mode, &str[0] );
}

void DebugAddWorldTextf( Mat44 basis, Vec2 pivot, Rgba8 color, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string str = Stringv( format, args );

	DebugAddWorldText( basis, pivot, color, color, 1.f, eDebugRenderMode::DEBUG_RENDER_USE_DEPTH, &str[0] );
}

void DebugAddWorldBillboardText( Vec3 origin, Vec2 pivot, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode, char const* text )
{
	DebugRenderObject_BillboardText* textObj = new DebugRenderObject_BillboardText( text, origin, pivot, start_color, end_color, duration, mode );

	g_debugRenderSystem->AddWorldObject( textObj );
}


void DebugAddWorldBillboardTextf( Vec3 origin, Vec2 pivot, Rgba8 color, float duration, eDebugRenderMode mode, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string str = Stringv( format, args );

	DebugAddWorldBillboardText( origin, pivot, color, color, duration, mode, &str[0] );
}

void DebugAddWorldBillboardTextf( Vec3 origin, Vec2 pivot, Rgba8 color, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string str = Stringv( format, args );

	DebugAddWorldBillboardText( origin, pivot, color, color, 1.f, eDebugRenderMode::DEBUG_RENDER_USE_DEPTH, &str[0] );
}

void DebugAddWorldGrid( Vec3 origin, Vec3 i, float iMin, float iMax, float iMinorSegment, float iMajorSegment, Rgba8 iMinorColor, Rgba8 iMajorColor, Rgba8 iOriginColor, 
	Vec3 j, float jMin, float jMax, float jMinorSegment, float jMajorSegment, Rgba8 jMinorcolor, Rgba8 jMajorColor, Rgba8 jOriginColor, Rgba8 axisColor )
{
	UNUSED( origin );
	UNUSED( i );
	UNUSED( iMin );
	UNUSED( iMax );
	UNUSED( iMinorSegment );
	UNUSED( iMajorSegment );
	UNUSED( iMinorColor );
	UNUSED( iMajorColor );
	UNUSED( iOriginColor );
	UNUSED( j );
	UNUSED( jMin );
	UNUSED( jMax );
	UNUSED( jMinorSegment );
	UNUSED( jMajorSegment );
	UNUSED( jMinorcolor );
	UNUSED( jMajorColor );
	UNUSED( jOriginColor );
	UNUSED( axisColor );
}

void DebugAddWorldXYGrid()
{
}

void DebugAddWorldXZGrid()
{
}

void DebugAddWorldYZGrid()
{
}

void DebugAddWireMeshToWorld( Mat44 transform, GPUMesh* mesh, Rgba8 start_tint, Rgba8 end_tint, float duration, eDebugRenderMode mode )
{
	UNUSED( transform );
	UNUSED( mesh );
	UNUSED( start_tint );
	UNUSED( end_tint );
	UNUSED( duration );
	UNUSED( mode );
}

void DebugAddWireMeshToWorld( Mat44 transform, GPUMesh* mesh, float duration, eDebugRenderMode mode )
{
	UNUSED( transform );
	UNUSED( mesh );
	UNUSED( duration );
	UNUSED( mode );
}

void DebugRenderSetScreenHeight( float height, float width )
{
	g_debugRenderSystem->m_screenHeight = height;
	g_debugRenderSystem->m_screenWidth = width;
}

AABB2 DebugGetScreenBounds()
{
	Vec2 min = Vec2::ZERO;
	Vec2 max = Vec2( g_debugRenderSystem->m_screenWidth, g_debugRenderSystem->m_screenHeight );
	return AABB2( min, max );
}

void DebugAddScreenPoint( Vec2 pos, float size, Rgba8 start_color, Rgba8 end_color, float duration )
{
	DebugRenderObject_ScreenPoint* pointObj = new DebugRenderObject_ScreenPoint( pos, size, start_color, end_color, duration );

	g_debugRenderSystem->AddScreenObject( pointObj );
}

void DebugAddScreenPoint( Vec2 pos, float size, Rgba8 color, float duration )
{
	DebugAddScreenPoint( pos, size, color, color, duration );
}

void DebugAddScreenPoint( Vec2 pos, Rgba8 color )
{
	DebugAddScreenPoint( pos, 1.f, color, color, 1.f );
}

void DebugAddScreenLine( Vec2 p0, Rgba8 p0_start_color, Rgba8 p0_end_color, Vec2 p1, Rgba8 p1_start_color, Rgba8 p1_end_color, float duration )
{
	DebugRenderObject_ScreenLine* lineObj = new DebugRenderObject_ScreenLine( p0, p0_start_color, p0_end_color, p1, p1_start_color, p1_end_color, duration );

	g_debugRenderSystem->AddScreenObject( lineObj );
}

void DebugAddScreenLine( Vec2 p0, Vec2 p1, Rgba8 color, float duration )
{
	DebugAddScreenLine( p0, color, color, p1, color, color, duration );
}

void DebugAddScreenArrow( Vec2 p0, Rgba8 p0_start_color, Rgba8 p0_end_color, Vec2 p1, Rgba8 p1_start_color, Rgba8 p1_end_color, float duration )
{
	DebugRenderObject_ScreenArrow* arrowObj = new DebugRenderObject_ScreenArrow( p0, p0_start_color, p0_end_color, p1, p1_start_color, p1_end_color, duration );

	g_debugRenderSystem->AddScreenObject( arrowObj );
}

void DebugAddScreenArrow( Vec2 p0, Vec2 p1, Rgba8 color, float duration )
{
	DebugAddScreenArrow( p0, color, color, p1, color, color, duration );
}

void DebugAddScreenQuad( AABB2 bounds, Rgba8 start_color, Rgba8 end_color, float duration )
{
	DebugRenderObject_ScreenQuad* quadObj = new DebugRenderObject_ScreenQuad( bounds, start_color, end_color, duration );

	g_debugRenderSystem->AddScreenObject( quadObj );
}

void DebugAddScreenQuad( AABB2 bounds, Rgba8 color, float duration )
{
	DebugAddScreenQuad( bounds, color, color, duration );
}

void DebugAddScreenTexturedQuad( AABB2 bounds, const Texture* tex, AABB2 uvs, Rgba8 start_tint, Rgba8 end_tint, float duration )
{
	DebugRenderObject_ScreenQuad* quadObj = new DebugRenderObject_ScreenQuad( bounds, start_tint, end_tint, duration, uvs, tex );

	g_debugRenderSystem->AddScreenObject( quadObj );
}

void DebugAddScreenTexturedQuad( AABB2 bounds, const Texture* tex, AABB2 uvs, Rgba8 tint, float duration )
{
	DebugAddScreenTexturedQuad( bounds, tex, uvs, tint, tint, duration );
}

void DebugAddScreenTexturedQuad( AABB2 bounds, const Texture* tex, Rgba8 tint, float duration )
{
	DebugAddScreenTexturedQuad( bounds, tex, AABB2::ZERO_TO_ONE, tint, duration );
}

void DebugAddScreenText( Vec4 pos, Vec2 pivot, float size, Rgba8 start_color, Rgba8 end_color, float duration, char const* text )
{
	Vec2 textDimension = g_theFont->GetDimensionsForText2D( size, text );
	Vec2 position = -textDimension * pivot;
	AABB2 screenBounds = DebugGetScreenBounds();
	Vec2 screenSize = screenBounds.GetDimensions();
	Vec2 rationPosition = Vec2( pos.x, pos.y );
	Vec2 offsetPosition = Vec2( pos.z, pos.w );

	Vec2 textBoundsMins = position + (rationPosition * screenSize) + offsetPosition;
	DebugRenderObject_ScreenText* textObj = new DebugRenderObject_ScreenText( text, textBoundsMins, size, start_color, end_color, duration );

	g_debugRenderSystem->AddScreenObject( textObj );
}

void DebugAddScreenTextf( Vec4 pos, Vec2 pivot, float size, Rgba8 start_color, Rgba8 end_color, float duration, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string str = Stringv( format, args );

	DebugAddScreenText( pos, pivot, size, start_color, end_color, duration, &str[0] );
}

void DebugAddScreenTextf( Vec4 pos, Vec2 pivot, float size, Rgba8 color, float duration, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string str = Stringv( format, args );

	DebugAddScreenTextf( pos, pivot, size, color, color, duration, &str[0] );
}

void DebugAddScreenTextf( Vec4 pos, Vec2 pivot, float size, Rgba8 color, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string str = Stringv( format, args );

	DebugAddScreenTextf( pos, pivot, size, color, color, 1.f, &str[0] );
}

void DebugAddScreenTextf( Vec4 pos, Vec2 pivot, Rgba8 color, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string str = Stringv( format, args );

	DebugAddScreenTextf( pos, pivot, 10.f, color, color, 1.f, &str[0] );
}

void DebugAddScreenBasis( Vec2 screen_origin_location, Mat44 basis_to_render, Rgba8 start_tint, Rgba8 end_tint, float duration )
{
	UNUSED(screen_origin_location);
	UNUSED(basis_to_render);
	UNUSED(start_tint);
	UNUSED(end_tint);
	UNUSED(duration);
}

void DebugAddScreenBasis( Vec2 screen_origin_location, Mat44 basis_to_render, Rgba8 tint, float duration )
{
	UNUSED( screen_origin_location );
	UNUSED( basis_to_render );
	UNUSED( tint );
	UNUSED( duration );
}

void DebugAddMessage( float duration, Rgba8 start_color, Rgba8 end_color, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string str = Stringv( format, args );

	DebugRenderObject_ScreenMessage* textObj = new DebugRenderObject_ScreenMessage( str, start_color, end_color, duration );

	g_debugRenderSystem->m_screenMessageObjectList.push_back( textObj );
}

void DebugAddMessage( float duration, Rgba8 color, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string str = Stringv( format, args );

	DebugAddMessage( duration, color, color, &str[0] );
}

void DebugAddMessage( Rgba8 color, char const* format, ... )
{
	va_list args;
	va_start( args, format );
	std::string str = Stringv( format, args );

	DebugAddMessage( 1.f, color, &str[0] );
}

void DebugRenderSystem::AddWorldObject( DebugRenderObject* obj )
{
	m_worldObjectList.push_back( obj );
}

void DebugRenderSystem::AddScreenObject( DebugRenderObject* obj )
{
	m_screenObjectList.push_back( obj );
}

void DebugRenderSystem::DeleteWorldObject( uint index )
{
	delete m_worldObjectList[index];
	m_worldObjectList[index] = nullptr;

	uint lastIndex = (uint) m_worldObjectList.size() - 1;
	m_worldObjectList[index] = m_worldObjectList[lastIndex];
	m_worldObjectList.pop_back();
}

void DebugRenderSystem::DeleteScreenObject( uint index )
{
	delete m_screenObjectList[index];
	m_screenObjectList[index] = nullptr;

	uint lastIndex = (uint)m_screenObjectList.size() - 1;
	m_screenObjectList[index] = m_screenObjectList[lastIndex];
	m_screenObjectList.pop_back();
}

COMMAND( debug_render, "Do you need render debug objects. enabled=true", "enabled" )
{
	g_debugRenderSystem->isRendering = args.GetValue( "enabled", true );
}

COMMAND( debug_add_world_point, "Add a world point. position=0,0,0 duration=5", "position,duration" )
{
	Vec3 position = args.GetValue( "position", Vec3::ZERO );
	float duration = args.GetValue( "duration", 1.f );

	DebugAddWorldPoint( position, Rgba8::WHITE, duration );
}

COMMAND( debug_add_world_wire_disc, "Add a world wire disc. position=0,0,0 radius=1 duration=5", "position,radius,duration" )
{
	Vec3 position = args.GetValue( "position", Vec3::ZERO );
	float radius = args.GetValue( "radius", 1.f );
	float duration = args.GetValue( "duration", 1.f );

	DebugAddWorldWireSphere( position, radius, Rgba8::WHITE, duration );
}

COMMAND( debug_add_world_wire_bounds, "Add a world wire box. min=0,0,0 max=1,1,1 duration=5", "min,max,duration" )
{
	Vec3 min = args.GetValue( "min", Vec3::ZERO );
	Vec3 max = args.GetValue( "max", Vec3::ZERO );
	float duration = args.GetValue( "duration", 1.f );

	DebugAddWorldWireBounds( AABB3( min, max ), Rgba8::WHITE, duration );
}

COMMAND( debug_add_world_billboard_text, "Add a world billboard text. position=0,0,0 pivot=0.5,0.5 text=TEXT", "position,pivot,text" )
{
	Vec3 position = args.GetValue( "position", Vec3::ZERO );
	Vec2 pivot = args.GetValue( "pivot", ALIGN_CENTERED );
	std::string text = args.GetValue( "text", "" );

	DebugAddWorldBillboardText( position, pivot, Rgba8::WHITE, Rgba8::WHITE, 5.f, eDebugRenderMode::DEBUG_RENDER_USE_DEPTH, &text[0] );
}

COMMAND( debug_add_screen_point, "Add a screen point. position=100,100 duration=5", "position,duration" )
{
	Vec2 position = args.GetValue( "position", Vec2::ZERO );
	float duration = args.GetValue( "duration", 1.f );

	DebugAddScreenPoint( position, 30.f, Rgba8::WHITE, duration );
}

COMMAND( debug_add_screen_quad, "Add a screen quad. min=0,0 max=100,100 duration=5", "min,max,duration" )
{
	Vec2 min = args.GetValue( "min", Vec2::ZERO );
	Vec2 max = args.GetValue( "max", Vec2::ZERO );
	float duration = args.GetValue( "duration", 1.f );

	DebugAddScreenQuad( AABB2( min, max ), Rgba8::WHITE, duration );
}

COMMAND( debug_add_screen_text, "Add a screen text. position=100,100 pivot=0.5,0.5 text=TEXT", "position,pivot,text" )
{
	Vec2 position = args.GetValue( "position", Vec2::ZERO );
	Vec2 pivot = args.GetValue( "pivot", ALIGN_CENTERED );
	std::string text = args.GetValue( "text", "" );

	DebugAddScreenText( Vec4( 0.f, 0.f, position.x, position.y ), pivot, 30.f, Rgba8::WHITE, Rgba8::WHITE, 5.f, &text[0] );
}

COMMAND( print_message, "Print a message on screem. text=MESSAGE duration=2 color=255,255,255", "duration,text,color" )
{
	float duration = args.GetValue( "duration", 2.f );
	std::string text = args.GetValue( "text", "" );
	Rgba8 color = args.GetValue( "color", Rgba8::WHITE );

	DebugAddMessage( duration, color, &text[0] );
}