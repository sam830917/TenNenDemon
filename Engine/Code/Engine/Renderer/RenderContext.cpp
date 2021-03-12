#include "RenderContext.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Todo.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/TextureCube.hpp"

#pragma warning(push ,3)
#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"
#pragma warning(pop)

#pragma comment( lib, "d3d11.lib" )         // needed a01
#pragma comment( lib, "dxgi.lib" )          // needed a01
#pragma comment( lib, "d3dcompiler.lib" )   // needed when we get to shaders

void RenderContext::Startup( Window* window )
{
	IDXGISwapChain* swapchain;

	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
	#if defined(RENDER_DEBUG)
		flags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	// define the swapchain
	DXGI_SWAP_CHAIN_DESC swapchainDesc;
	memset( &swapchainDesc, 0, sizeof(swapchainDesc) );
	swapchainDesc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // on swap, the old buffer is discarded
	swapchainDesc.Flags = 0;

	HWND hwnd = (HWND) window->m_hwnd;
	swapchainDesc.OutputWindow = hwnd; // HWND for the window to be used
	swapchainDesc.SampleDesc.Count = 1;

	swapchainDesc.Windowed = TRUE;
	swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
	swapchainDesc.BufferDesc.Width = window->GetClientWidth();
	swapchainDesc.BufferDesc.Height = window->GetClientHeight();

	HRESULT result = D3D11CreateDeviceAndSwapChain( nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, 
		nullptr, 
		flags, // controls the type of device we make
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapchainDesc,
		&swapchain,
		&m_device,
		nullptr,
		&m_context );

	GUARANTEE_OR_DIE( SUCCEEDED(result), "Fail to creating Render Context" );

	m_swapchain = new SwapChain( this, swapchain);
	m_swapchain->GetBackBuffer();
	m_defaultShader = CreateOrGetShader( "Data/Shaders/Default.hlsl" );

	m_frameUBO = new RenderBuffer( this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	m_modelUBO = new RenderBuffer( this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	m_lightUBO = new RenderBuffer( this, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );

	m_texDefultColor = new Texture( Rgba8::WHITE, this );
	float normalDefualt[4] = { 0.5f, 0.5f, 1.f, 1.f };
	m_texDefultNormal = new Texture( Rgba8( 127, 127,255,0 ), this );
	m_defultSampler = new Sampler( this, SAMPLER_POINT );
	CreateBlendState();
	m_immediateMesh = new GPUMesh( this );
	m_defultBackBuffer = Texture::CreateDepthStencilBuffer( this, window->GetClientWidth(), window->GetClientHeight() );
	DebugRenderSetScreenHeight( (float)window->GetClientHeight(), (float)window->GetClientWidth() );
	m_effectCamera = new Camera();
	m_effectCamera->SetClearMode( CLEAR_NONE );
}

void RenderContext::BeginFrame()
{
}

void RenderContext::EndFrame()
{
	m_swapchain->Present();
}

void RenderContext::Shutdown()
{
	delete m_effectCamera;
	m_effectCamera = nullptr;

	GUARANTEE_OR_DIE( m_renderTargetPool.size() == m_totalRenderTargetMade, "RenderTargetPool not fully released." );
	for( Texture* tex : m_renderTargetPool ) {
		delete tex;
		tex = nullptr;
	}
	m_renderTargetPool.clear();

	delete m_immediateMesh;
	m_immediateMesh = nullptr;
	delete m_swapchain;
	m_swapchain = nullptr;
	delete m_frameUBO;
	m_frameUBO = nullptr;
	delete m_modelUBO;
	m_modelUBO = nullptr;
	delete m_lightUBO;
	m_lightUBO = nullptr;
	delete m_defultSampler;
	m_defultSampler = nullptr;
	delete m_texDefultColor;
	m_texDefultColor = nullptr;
	delete m_texDefultNormal;
	m_texDefultNormal = nullptr;
	delete m_defultBackBuffer;
	m_defultBackBuffer = nullptr;

	for ( int texIdx = 0; texIdx < (int) m_textureList.size(); texIdx++ )
	{
		if ( m_textureList[texIdx] != nullptr )
		{
			delete m_textureList[texIdx];
			m_textureList[texIdx] = nullptr;
		}
	}

	for( int shaderIdx = 0; shaderIdx < (int)m_shaderList.size(); shaderIdx++ )
	{
		if( m_shaderList[shaderIdx] != nullptr )
		{
			delete m_shaderList[shaderIdx];
			m_shaderList[shaderIdx] = nullptr;
		}
	}

	//ID3D11Debug* d3dDebug;
	//HRESULT hr = m_device->QueryInterface( __uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug) );
	//if( SUCCEEDED( hr ) ) {
	//	hr = d3dDebug->ReportLiveDeviceObjects( D3D11_RLDO_DETAIL );
	//}
	//DX_SAFE_RELEASE( d3dDebug );

	DX_SAFE_RELEASE( m_currentRasterState );
	DX_SAFE_RELEASE( m_alphaBlendState );
	DX_SAFE_RELEASE( m_additiveBlendState );
	DX_SAFE_RELEASE( m_opaqeBlendState );
	DX_SAFE_RELEASE( m_depthStencilState );
	DX_SAFE_RELEASE( m_device );
	DX_SAFE_RELEASE( m_context );
}

void RenderContext::ClearScreen( const Rgba8& clearColor )
{
	float clearFloats[4];
	clearFloats[0] = clearColor.r / 255.f;
	clearFloats[1] = clearColor.g / 255.f;
	clearFloats[2] = clearColor.b / 255.f;
	clearFloats[3] = clearColor.a / 255.f;

	Texture* backBuffer = m_swapchain->GetBackBuffer();
	TextureView* backbuffer_rtv = backBuffer->GetOrCreateRenderTargetView();

	ID3D11RenderTargetView* rtv = backbuffer_rtv->GetAsRTV();
	m_context->ClearRenderTargetView( rtv, clearFloats );
}

void RenderContext::BeginCamera( const Camera& camera )
{
	m_isDrawing = true;
#if defined(RENDER_DEBUG)
	m_context->ClearState();
#endif

	Texture* cameraColorTarget = camera.GetColorTarget();
	if ( cameraColorTarget == nullptr )
	{
		cameraColorTarget = m_swapchain->GetBackBuffer();
	}
	IntVec2 outputSize = cameraColorTarget->GetDimensions();

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)outputSize.x;
	viewport.Height = (float)outputSize.y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	std::vector<ID3D11RenderTargetView*> rtvList;
	int rtvCount = camera.GetColorTargetCount();
	rtvList.resize( camera.GetColorTargetCount() );
	for ( int i = 0; i < rtvCount; ++i )
	{
		rtvList[i] = nullptr;

		Texture* colorTarget = camera.GetColorTarget(i);
		if ( colorTarget != nullptr )
		{
			TextureView* rTextureView = colorTarget->GetOrCreateRenderTargetView();
			rtvList[i] = rTextureView->GetAsRTV();
		}
	}

	Texture* depthStencilTarget = camera.GetDepthStencilBuffer();
	ID3D11DepthStencilView* dsv = nullptr;
	if( depthStencilTarget == nullptr )
	{
		depthStencilTarget = m_defultBackBuffer;
	}
	dsv = depthStencilTarget->GetOrCreateDepthStencilView()->GetAsDSV();

	m_context->RSSetViewports( 1, &viewport );

	if( rtvCount == 0 )
	{
		TextureView* view = cameraColorTarget->GetOrCreateRenderTargetView();
		ID3D11RenderTargetView* rtv = view->GetAsRTV();
		m_context->OMSetRenderTargets( 1, &rtv, dsv );

		if( eCameraClearBitlag::CLEAR_COLOR_BIT & camera.GetClearMode() )
		{
			ClearScreen( camera.GetClearColor() );
		}
	}
	else
	{
		m_context->OMSetRenderTargets( rtvCount, rtvList.data(), dsv );
	}

	if ( eCameraClearBitlag::CLEAR_COLOR_BIT & camera.GetClearMode() )
	{
		//ClearScreen( camera.GetClearColor() );
		Rgba8 clearColor = camera.GetClearColor();
		float clearFloats[4];
		clearFloats[0] = clearColor.r / 255.f;
		clearFloats[1] = clearColor.g / 255.f;
		clearFloats[2] = clearColor.b / 255.f;
		clearFloats[3] = clearColor.a / 255.f;

		for ( int i = 0; i < rtvCount; ++i )
		{
			if ( rtvList[i] != nullptr )
			{
				m_context->ClearRenderTargetView( rtvList[i], clearFloats );
			}
		}
	}
	m_lastVBO = nullptr;

	CreateDefaultRasterState();
	SetModelMatrix( Mat44::IDENTITY );
	SetModelTint( Rgba8::WHITE );

	BindUniformBuffer( 0, m_frameUBO );
	BindUniformBuffer( 1, camera.GetAndUpdateUBO() );
	BindShader( nullptr );
	BindTexture( nullptr );
	BindNormalTexture( nullptr );
	BindSampler( nullptr );

	SetBlendMode( eBlendMode::BLEND_ALPHA );
	DisableDepth();
	ClearDepth( depthStencilTarget, camera.GetClearDepth(), camera.GetClearStencil() );
}

void RenderContext::EndCamera( const Camera& camera )
{
	UNUSED( camera );
	DX_SAFE_RELEASE( m_currentRasterState );
	DX_SAFE_RELEASE( m_depthStencilState );
}

void RenderContext::UpdateFrameTime( float deltaTime )
{
	frame_data_t frameData;
	frameData.system_time = (float) GetCurrentTimeSeconds();
	frameData.system_delta_time = deltaTime;

	m_frameUBO->Update( &frameData, sizeof( frameData ), sizeof( frameData ) );
}

void RenderContext::ClearDepth( Texture* depthStencilTexture, float depth, uint stencil )
{
	TextureView* view = depthStencilTexture->GetOrCreateDepthStencilView();
	ID3D11DepthStencilView* dsv = view->GetAsDSV();
	m_context->ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH, depth, (UINT8) stencil );
}

void RenderContext::BindTexture( const Texture* texture )
{
	Texture* tex = const_cast<Texture*>( texture );
	if ( tex == nullptr )
	{
		tex = m_texDefultColor;
	}

	TextureView* shaderResourceView = tex->GetOrCreateShaderResourceView();
	ID3D11ShaderResourceView* srvHandle = shaderResourceView->GetAsSRV();
	m_context->PSSetShaderResources( 0, 1, &srvHandle );
}

void RenderContext::BindTextureCube( const TextureCube* texture )
{
	TextureCube* tex = const_cast<TextureCube*>(texture);
	if( texture == nullptr )
	{
		return;
	}

	TextureView* shaderResourceView = tex->GetOrCreateShaderResourceView();
	ID3D11ShaderResourceView* srvHandle = shaderResourceView->GetAsSRV();
	m_context->PSSetShaderResources( 6, 1, &srvHandle );
}

void RenderContext::BindNormalTexture( const Texture* texture )
{
	Texture* tex = const_cast<Texture*>(texture);
	if( tex == nullptr )
	{
		tex = m_texDefultNormal;
	}

	TextureView* shaderResourceView = tex->GetOrCreateShaderResourceView();
	ID3D11ShaderResourceView* srvHandle = shaderResourceView->GetAsSRV();
	m_context->PSSetShaderResources( 1, 1, &srvHandle );
}

void RenderContext::BindTextureOnSlot( uint slot, const Texture* texture )
{
	Texture* tex = const_cast<Texture*>(texture);
	if( tex )
	{
		TextureView* shaderResourceView = tex->GetOrCreateShaderResourceView();
		ID3D11ShaderResourceView* srvHandle = shaderResourceView->GetAsSRV();
		m_context->PSSetShaderResources( slot, 1, &srvHandle );
	}
}

void RenderContext::BindSampler( Sampler* sampler )
{
	if( sampler == nullptr )
	{
		sampler = m_defultSampler;
	}
	ID3D11SamplerState* samplerHandle = sampler->GetHandle();
	m_context->PSSetSamplers( 0, 1, &samplerHandle );
}

void RenderContext::BindSamplerOnSlot( uint slot, Sampler* sampler )
{
	if( sampler == nullptr )
	{
		sampler = m_defultSampler;
	}
	ID3D11SamplerState* samplerHandle = sampler->GetHandle();
	m_context->PSSetSamplers( slot, 1, &samplerHandle );
}

void RenderContext::BindShader( Shader* shader )
{
	ASSERT_OR_DIE( IsDrawing(), "ERROR!" );

	m_currentShader = shader;
	if ( m_currentShader == nullptr )
	{
		m_currentShader = m_defaultShader;
	}

	m_context->VSSetShader( m_currentShader->m_vertexStage.m_vs, nullptr, 0 );
	m_context->RSSetState( m_currentRasterState );
	m_context->PSSetShader( m_currentShader->m_fragmentStage.m_fs, nullptr, 0 );
	m_shaderHasChanged = true;
}

void RenderContext::BindShaderFromFile( char const* filename )
{
	Shader* shader = CreateOrGetShader( filename );
	BindShader( shader );
}

Shader* RenderContext::CreateOrGetShader( char const* filename )
{
	for( int index = 0; index < (int) m_shaderList.size(); index++ )
	{
		if( strcmp(m_shaderList[index]->GetFilePath(), filename) == 0 )
		{
			return m_shaderList[index];
		}
	}
	Shader* shader = new Shader( this );
	shader->CreateFromFile( filename );
	m_shaderList.push_back( shader );
	return shader;
}

void RenderContext::BindUniformBuffer( uint slot, RenderBuffer* ubo )
{
	if ( ubo == nullptr )
	{
		return;
	}
	ID3D11Buffer* uboHandle = ubo->m_handle /*ubo->GetHandle()*/;

	m_context->VSSetConstantBuffers( slot, 1, &uboHandle );
	m_context->PSSetConstantBuffers( slot, 1, &uboHandle );
}

void RenderContext::BindUniformBuffer( uint slot, const Camera* camera )
{
	ID3D11Buffer* uboHandle = camera->GetUBO()->m_handle /*ubo->GetHandle()*/;

	m_context->VSSetConstantBuffers( slot, 1, &uboHandle );
	m_context->PSSetConstantBuffers( slot, 1, &uboHandle );
}

void RenderContext::BindVertexBuffer( uint slot, VertexBuffer* vertexBuffer )
{
	ID3D11Buffer* vboHandle = vertexBuffer->m_handle;
	UINT stride = (UINT)vertexBuffer->m_element_stride;
	UINT offset = 0;

	if( m_lastVBO != vboHandle )
	{
		m_context->IASetVertexBuffers( slot, 1, &vboHandle, &stride, &offset );
		m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		m_lastVBO = vboHandle;
		m_currentLayout = vertexBuffer->m_layout;
	}
}

void RenderContext::BindIndexBuffer( IndexBuffer* indexBuffer )
{
	ID3D11Buffer* uboHandle = indexBuffer->m_handle;
	m_context->IASetIndexBuffer( uboHandle, DXGI_FORMAT_R32_UINT, 0 );
}

void RenderContext::BindShaderStateFromName( const std::string& name )
{
	ShaderState* shaderState = ShaderState::GetDefinitions( name );
	if ( shaderState == nullptr )
	{
		ERROR_AND_DIE( Stringf( "Shader State \"%s\" not found.", name.c_str() ) );
	}
	BindShaderState( shaderState );
}

void RenderContext::BindShaderState( ShaderState* shaderState )
{
	if ( shaderState == nullptr )
	{
		return;
	}

	BindShader( shaderState->m_shader );
	SetBlendMode( shaderState->m_blendMode );
	EnableDepth( shaderState->m_depth, shaderState->m_writeDepth );
	SetFrontFaceWindOrder( shaderState->m_windingOrder );
	SetCullMode( shaderState->m_cullMode );
	SetFillMode( shaderState->m_fillMode );
}

void RenderContext::BindMaterial( Material* mat )
{
	BindShaderState( mat->m_shaderState );

	for ( std::map<uint, Texture*>::iterator it = mat->m_textureMap.begin(); it != mat->m_textureMap.end(); ++it )
	{
		uint slot = it->first;
		Texture* tex = it->second;
		BindTextureOnSlot( slot, tex );
	}

	for( std::map<uint, Sampler*>::iterator it = mat->m_samplerMap.begin(); it != mat->m_samplerMap.end(); ++it )
	{
		uint slot = it->first;
		Sampler* sampler = it->second;
		BindSamplerOnSlot( slot, sampler );
	}

	SetSpecularFactor( mat->m_specularFactor );
	SetSpecularPower( mat->m_specularPower );
	SetModelTint( mat->m_tint );
	mat->UpdateUBOIfDirty();
	BindMaterialBuffer( mat->m_ubo );
}

void RenderContext::BindMaterialFromName( const std::string& name )
{
	Material* mat = Material::GetDefinitions( name );
	BindMaterial( mat );
}

Texture* RenderContext::CreateOrGetTextureFromFile( const char* imageFilePath )
{
	for (int index = 0;index < (int) m_textureList.size();index++)
	{
		if (m_textureList[index]->GetFilePath() == imageFilePath)
		{
			return m_textureList[index];
		}
	}
	return CreateTextureFromFile( imageFilePath );
}

Texture* RenderContext::CreateTextureFromColor( Rgba8 color )
{
	return nullptr;
}

void RenderContext::Draw( int numVertexes, int vertexOffset )
{
	// describe the vertex format to the shader
	//ID3D11InputLayout* inputLayout = m_currentShader->GetOrCreateInputLayout();
	//m_context->IASetInputLayout( inputLayout );

	m_context->Draw( numVertexes, vertexOffset );
}

void RenderContext::DrawIndexed( int indexCount, int vertexOffset, int baseVertexOffset )
{
	//ID3D11InputLayout* inputLayout = m_currentShader->GetOrCreateInputLayout();
	//m_context->IASetInputLayout( inputLayout );

	m_context->DrawIndexed( indexCount, vertexOffset, baseVertexOffset );
}

void RenderContext::DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes )
{
	m_immediateMesh->UpdateVertices( numVertexes, vertexes );
	m_immediateMesh->UpdateIndices( 0, nullptr );  // ClearIndices()

	DrawMesh( m_immediateMesh );
}

void RenderContext::DrawVertexArray( const std::vector<Vertex_PCU> vertexes )
{
	if ( (int) vertexes.size() == 0 )
		return;

	DrawVertexArray( (int) vertexes.size(), &vertexes[0] );
}

void RenderContext::DrawLine( const Vec2& startVec2, const Vec2& endVec2, const Rgba8& color, float thickness )
{
	const float r = thickness * 0.5f;

	Vec2 fwd = endVec2 - startVec2;
	fwd.SetLength( r );
	Vec2 leftDisplacement = fwd.GetRotated90Degrees();

	const Vec2 endLeft = endVec2 + fwd + leftDisplacement;
	const Vec2 endRight = endVec2 + fwd - leftDisplacement;
	const Vec2 startLeft = startVec2 - fwd + leftDisplacement;
	const Vec2 startRight = startVec2 - fwd - leftDisplacement;

	Vertex_PCU lineVertexex[] =
	{
		Vertex_PCU( Vec3( startLeft.x, startLeft.y, 0.f ), color, Vec2() ),
		Vertex_PCU( Vec3( startRight.x, startRight.y, 0.f ), color, Vec2() ),
		Vertex_PCU( Vec3( endLeft.x, endLeft.y, 0.f ), color, Vec2() ),

		Vertex_PCU( Vec3( endRight.x, endRight.y, 0.f ), color, Vec2() ),
		Vertex_PCU( Vec3( startRight.x, startRight.y, 0.f ), color, Vec2() ),
		Vertex_PCU( Vec3( endLeft.x, endLeft.y, 0.f ), color, Vec2() )
	};
	constexpr int NUM_VERTEXES = sizeof( lineVertexex ) / sizeof( lineVertexex[0] );

	DrawVertexArray( NUM_VERTEXES, lineVertexex );
}

void RenderContext::DrawLinesFromPoints( int pointsCount, const Vec2* points, const Rgba8& color, float thickness )
{
	if( pointsCount > 1 )
	{
		for( int index = 0; index < pointsCount - 1; index++ )
		{
			const Vec2& startPoint = points[index];
			const Vec2& endPoint = points[index+1];
			DrawLine( startPoint, endPoint, color, thickness );
		}
		const Vec2& startPoint = points[0];
		const Vec2& endPoint = points[pointsCount - 1];
		DrawLine( startPoint, endPoint, color, thickness );
	}
}

void RenderContext::DrawAABB2D( const AABB2& bounds, const Rgba8& tint )
{
	Vec2 minVec2 = bounds.mins;
	Vec2 maxVec2 = bounds.maxs;

	Vertex_PCU vertexex[] =
	{
		Vertex_PCU( Vec3( minVec2.x, minVec2.y, 0.f ), tint, Vec2( 0.f, 0.f ) ),
		Vertex_PCU( Vec3( maxVec2.x, minVec2.y, 0.f ), tint, Vec2( 1.f, 0.f ) ),
		Vertex_PCU( Vec3( minVec2.x, maxVec2.y, 0.f ), tint, Vec2( 0.f, 1.f ) ),

		Vertex_PCU( Vec3( minVec2.x, maxVec2.y, 0.f ), tint, Vec2( 0.f, 1.f ) ),
		Vertex_PCU( Vec3( maxVec2.x, minVec2.y, 0.f ), tint, Vec2( 1.f, 0.f ) ),
		Vertex_PCU( Vec3( maxVec2.x, maxVec2.y, 0.f ), tint, Vec2( 1.f, 1.f ) )
	};
	constexpr int NUM_VERTEXES = sizeof( vertexex ) / sizeof( vertexex[0] );

	DrawVertexArray( NUM_VERTEXES, vertexex );
}

void RenderContext::DrawQuad( const Vec3& mins, const Vec3& maxs, const Rgba8& tint )
{
	Vertex_PCU vertexex[] =
	{
		Vertex_PCU( Vec3( mins.x, mins.y, mins.z ), tint, Vec2( 0.f, 0.f ) ),
		Vertex_PCU( Vec3( maxs.x, mins.y, maxs.z ), tint, Vec2( 1.f, 0.f ) ),
		Vertex_PCU( Vec3( mins.x, maxs.y, mins.z ), tint, Vec2( 0.f, 1.f ) ),

		Vertex_PCU( Vec3( mins.x, maxs.y, mins.z ), tint, Vec2( 0.f, 1.f ) ),
		Vertex_PCU( Vec3( maxs.x, mins.y, maxs.z ), tint, Vec2( 1.f, 0.f ) ),
		Vertex_PCU( Vec3( maxs.x, maxs.y, maxs.z ), tint, Vec2( 1.f, 1.f ) )
	};
	constexpr int NUM_VERTEXES = sizeof( vertexex ) / sizeof( vertexex[0] );

	DrawVertexArray( NUM_VERTEXES, vertexex );
}

void RenderContext::DrawAABB2D( const AABB2& bounds, const AABB2& spriteSheetUV, const Rgba8& tint )
{
	Vec2 minVec2 = bounds.mins;
	Vec2 maxVec2 = bounds.maxs;
	Vec2 minUVVec2 = spriteSheetUV.mins;
	Vec2 maxUVVec2 = spriteSheetUV.maxs;

	Vertex_PCU vertexex[] =
	{
		Vertex_PCU( Vec3( minVec2.x, minVec2.y, 0.f ), tint, Vec2( minUVVec2.x, minUVVec2.y ) ),
		Vertex_PCU( Vec3( minVec2.x, maxVec2.y, 0.f ), tint, Vec2( minUVVec2.x, maxUVVec2.y ) ),
		Vertex_PCU( Vec3( maxVec2.x, minVec2.y, 0.f ), tint, Vec2( maxUVVec2.x, minUVVec2.y ) ),

		Vertex_PCU( Vec3( maxVec2.x, minVec2.y, 0.f ), tint, Vec2( maxUVVec2.x, minUVVec2.y ) ),
		Vertex_PCU( Vec3( minVec2.x, maxVec2.y, 0.f ), tint, Vec2( minUVVec2.x, maxUVVec2.y ) ),
		Vertex_PCU( Vec3( maxVec2.x, maxVec2.y, 0.f ), tint, Vec2( maxUVVec2.x, maxUVVec2.y ) )
	};
	constexpr int NUM_VERTEXES = sizeof( vertexex ) / sizeof( vertexex[0] );

	DrawVertexArray( NUM_VERTEXES, vertexex );
}

void RenderContext::DrawPolyGon2D( const Polygon2& polygon, const Rgba8& tint )
{
	if( polygon.IsValid() )
	{
		std::vector<Vec2> points = polygon.GetPoints();

		const int triangleNumbers = polygon.GetVertexCount() - 2;
		const int vertexNumbers = triangleNumbers * 3;
		std::vector<Vertex_PCU> vertexes;
		vertexes.reserve( vertexNumbers );
		for( int index = 0; index < triangleNumbers; index++ )
		{
			vertexes.push_back( Vertex_PCU( Vec3( points[0].x, points[0].y, 0.f ), tint, Vec2() ) );
			vertexes.push_back( Vertex_PCU( Vec3( points[index+1].x, points[index+1].y, 0.f ), tint, Vec2() ) );
			vertexes.push_back( Vertex_PCU( Vec3( points[index+2].x, points[index+2].y, 0.f ), tint, Vec2() ) );
		}

		DrawVertexArray( (int)vertexes.size(), &vertexes[0] );
	}
}

void RenderContext::DrawCapsule2( const Vec2& boneStart, const Vec2& boneEnd, float radius, const Rgba8& color )
{
	Vec2 displacement = boneEnd - boneStart;
	displacement.Rotate90Degrees();
	Vec2 iBasisNormal = displacement.GetNormalized();
	Vec2 iBasis = iBasisNormal * radius;

	Vec2 bottomLeftVec2	 = boneStart - iBasis;
	Vec2 bottomRightVec2 = boneEnd - iBasis;
	Vec2 topLeftVec2	 = boneStart + iBasis;
	Vec2 topRightVec2	 = boneEnd + iBasis;

	Vertex_PCU vertexex[] =
	{
		Vertex_PCU( Vec3( bottomLeftVec2.x, bottomLeftVec2.y, 0.f ), color, Vec2( 0.f, 0.f ) ),
		Vertex_PCU( Vec3( topLeftVec2.x, topLeftVec2.y, 0.f ), color, Vec2( 0.f, 1.f ) ),
		Vertex_PCU( Vec3( bottomRightVec2.x, bottomRightVec2.y, 0.f ), color, Vec2( 1.f, 0.f ) ),

		Vertex_PCU( Vec3( bottomRightVec2.x, bottomRightVec2.y, 0.f ), color, Vec2( 1.f, 0.f ) ),
		Vertex_PCU( Vec3( topLeftVec2.x, topLeftVec2.y, 0.f ), color, Vec2( 0.f, 1.f ) ),
		Vertex_PCU( Vec3( topRightVec2.x, topRightVec2.y, 0.f ), color, Vec2( 1.f, 1.f ) )
	};
	constexpr int NUM_VERTEXES = sizeof( vertexex ) / sizeof( vertexex[0] );
	DrawCircle( boneStart, radius, color, iBasisNormal.GetAngleDegrees(), 180.f );
	DrawCircle( boneEnd, radius, color, iBasisNormal.GetAngleDegrees() + 180.f, 180.f );

	DrawVertexArray( NUM_VERTEXES, vertexex );
}

void RenderContext::DrawRing( const Vec2& centerVec2, float radius, const Rgba8& color, float thickness )
{
	const int verTexesNum = 64;
	Vec2 innerCircleArray[verTexesNum];
	Vec2 outterCircleArray[verTexesNum];
	const float r = thickness * 0.5f;

	float degrees = 0;
	for( int vertexIndex = 0; vertexIndex < verTexesNum; vertexIndex++ )
	{
		Vec2 innerCircle = TransformPosition2D( Vec2( radius - r, 0.f ), 1.f, degrees, centerVec2 );
		Vec2 outterCircle = TransformPosition2D( Vec2( radius + r, 0.f ), 1.f, degrees, centerVec2 );
		innerCircleArray[vertexIndex] = innerCircle;
		outterCircleArray[vertexIndex] = outterCircle;
		degrees += 360.f / verTexesNum;
	}

	Vertex_PCU ringVertexes[verTexesNum * 6];

	for( int vertexIndex = 0; vertexIndex < verTexesNum; vertexIndex++ )
	{
		const int lineVertexIdx = vertexIndex * 6;

		ringVertexes[lineVertexIdx] = Vertex_PCU( Vec3( innerCircleArray[vertexIndex].x, innerCircleArray[vertexIndex].y, 0.f ), color, Vec2() );
		if( (vertexIndex + 1) == verTexesNum )
		{
			ringVertexes[lineVertexIdx + 1] = Vertex_PCU( Vec3( innerCircleArray[0].x, innerCircleArray[0].y, 0.f ), color, Vec2() );
			ringVertexes[lineVertexIdx + 2] = Vertex_PCU( Vec3( outterCircleArray[0].x, outterCircleArray[0].y, 0.f ), color, Vec2() );

			ringVertexes[lineVertexIdx + 3] = Vertex_PCU( Vec3( outterCircleArray[0].x, outterCircleArray[0].y, 0.f ), color, Vec2() );
		}
		else
		{
			ringVertexes[lineVertexIdx + 1] = Vertex_PCU( Vec3( innerCircleArray[vertexIndex + 1].x, innerCircleArray[vertexIndex + 1].y, 0.f ), color, Vec2() );
			ringVertexes[lineVertexIdx + 2] = Vertex_PCU( Vec3( outterCircleArray[vertexIndex + 1].x, outterCircleArray[vertexIndex + 1].y, 0.f ), color, Vec2() );

			ringVertexes[lineVertexIdx + 3] = Vertex_PCU( Vec3( outterCircleArray[vertexIndex + 1].x, outterCircleArray[vertexIndex + 1].y, 0.f ), color, Vec2() );
		}
		ringVertexes[lineVertexIdx + 4] = Vertex_PCU( Vec3( outterCircleArray[vertexIndex].x, outterCircleArray[vertexIndex].y, 0.f ), color, Vec2() );
		ringVertexes[lineVertexIdx + 5] = Vertex_PCU( Vec3( innerCircleArray[vertexIndex].x, innerCircleArray[vertexIndex].y, 0.f ), color, Vec2() );
	}
	constexpr int NUM_VERTEXES = sizeof( ringVertexes ) / sizeof( ringVertexes[0] );

	DrawVertexArray( NUM_VERTEXES, ringVertexes );
}

void RenderContext::DrawCircle( const Vec2& centerVec2, float radius, const Rgba8& color, float rotation, float renderDegrees )
{
	const int vertexNumber = 64;
	Vec3 vertexsVec3[vertexNumber + 1];
	float degrees = 0.f;
	float degreesPerVertex = renderDegrees / vertexNumber;

	for( int vertexIndex = 0; vertexIndex < vertexNumber + 1; vertexIndex++ )
	{
		Vec3 newVec3 = Vec3( radius, 0.f, 0.f );
		vertexsVec3[vertexIndex] = newVec3.GetRotatedAboutZDegrees( degrees );
		degrees += degreesPerVertex;
	}

	// Create VertexPCU
	Vertex_PCU vertexsArray[vertexNumber * 3];
	for( int triangleIndex = 0, localVertexsIndex = 0; triangleIndex < vertexNumber; triangleIndex++ )
	{
		if ( triangleIndex + 1 == vertexNumber && renderDegrees == 360.f )
		{
			vertexsArray[localVertexsIndex++] = Vertex_PCU( vertexsVec3[triangleIndex], color, Vec2() );
			vertexsArray[localVertexsIndex++] = Vertex_PCU( Vec3(), color, Vec2() );
			vertexsArray[localVertexsIndex++] = Vertex_PCU( vertexsVec3[0], color, Vec2() );
		}
		else
		{
			vertexsArray[localVertexsIndex++] = Vertex_PCU( vertexsVec3[triangleIndex], color, Vec2() );
			vertexsArray[localVertexsIndex++] = Vertex_PCU( Vec3(), color, Vec2() );
			vertexsArray[localVertexsIndex++] = Vertex_PCU( vertexsVec3[triangleIndex + 1], color, Vec2() );
		}
	}

	constexpr int NUM_VERTEXES = sizeof( vertexsArray ) / sizeof( vertexsArray[0] );
	TransformVertexArray( NUM_VERTEXES, vertexsArray, 1.f, rotation, centerVec2 );

	DrawVertexArray( NUM_VERTEXES, vertexsArray );
}

void RenderContext::DrawText2D( const std::string& text, const Vec2& position, float size, BitmapFont* font, const Rgba8& color )
{
	std::vector<Vertex_PCU> textVerts;
	font->AddVertsForText2D( textVerts, position, size, text, color );
	BindTexture( font->GetTexture() );
	DrawVertexArray( textVerts );
}

void RenderContext::DrawMesh( GPUMesh* mesh )
{
	BindVertexBuffer( 0, mesh->GetVertexBuffer() );
	// m_context->IASetInputLayout
	// this needs both a shader and a vertex format
	UpdateLayoutIfNeeded(); // based on current vertex buffer and current shader
	
	bool hasIndices = mesh->GetIndexCount() > 0;
	if( hasIndices )
	{
		BindIndexBuffer( mesh->GetIndexBuffer() );
		DrawIndexed( mesh->GetIndexCount(), 0, 0 );
	}
	else {
		Draw( mesh->GetVertexCount(), 0 );
	}
}

void RenderContext::SetAmbientColor( const Rgba8& color )
{
	m_lightConstants.ambient[0] = color.r / 255.f;
	m_lightConstants.ambient[1] = color.g / 255.f;
	m_lightConstants.ambient[2] = color.b / 255.f;
}

void RenderContext::SetAmbientIntensity( float intensity )
{
	m_lightConstants.ambient[3] = intensity;
}

void RenderContext::SetAmbientLight( const Rgba8& color, float intensity )
{
	m_lightConstants.ambient[0] = color.r / 255.f;
	m_lightConstants.ambient[1] = color.g / 255.f;
	m_lightConstants.ambient[2] = color.b / 255.f;
	m_lightConstants.ambient[3] = intensity;
	m_lightUBO->Update( &m_lightConstants, sizeof( m_lightConstants ), sizeof( m_lightConstants ) );
	BindUniformBuffer( 4, m_lightUBO );
}

void RenderContext::SetSpecularFactor( float specularFactor )
{
	m_lightConstants.specularFactor = specularFactor;
	m_lightUBO->Update( &m_lightConstants, sizeof( m_lightConstants ), sizeof( m_lightConstants ) );
	BindUniformBuffer( 4, m_lightUBO );
}

void RenderContext::SetSpecularPower( float specularPower )
{
	m_lightConstants.specularPower = specularPower;
	m_lightUBO->Update( &m_lightConstants, sizeof( m_lightConstants ), sizeof( m_lightConstants ) );
	BindUniformBuffer( 4, m_lightUBO );
}

void RenderContext::BindMaterialBuffer( RenderBuffer* buf )
{
	BindUniformBuffer( 5, buf );
}

void RenderContext::EnableLight( uint idx, light_t const& lightInfo )
{
	m_lightConstants.lights[idx] = lightInfo;
	m_lightUBO->Update( &m_lightConstants, sizeof( m_lightConstants ), sizeof( m_lightConstants ) );
	BindUniformBuffer( 4, m_lightUBO );
}

void RenderContext::DisableLight( uint idx )
{
	UNUSED( idx );
	SetAmbientIntensity(0.f);
}

void RenderContext::EnableFog( float nearFog, float farFog, const Rgba8& nearFogColor, const Rgba8& farFogColor )
{
	m_lightConstants.fog_intensity = 1.f;
	m_lightConstants.fog_near_distance = nearFog;
	m_lightConstants.fog_far_distance = farFog;
	m_lightConstants.fog_near_color[0] = nearFogColor.r / 255.f;
	m_lightConstants.fog_near_color[1] = nearFogColor.g / 255.f;
	m_lightConstants.fog_near_color[2] = nearFogColor.b / 255.f;
	m_lightConstants.fog_far_color[0] = farFogColor.r / 255.f;
	m_lightConstants.fog_far_color[1] = farFogColor.g / 255.f;
	m_lightConstants.fog_far_color[2] = farFogColor.b / 255.f;

	m_lightUBO->Update( &m_lightConstants, sizeof( m_lightConstants ), sizeof( m_lightConstants ) );
	BindUniformBuffer( 4, m_lightUBO );
}

void RenderContext::DisableFog()
{
	m_lightConstants.fog_intensity = 0.f;

	m_lightUBO->Update( &m_lightConstants, sizeof( m_lightConstants ), sizeof( m_lightConstants ) );
	BindUniformBuffer( 4, m_lightUBO );
}

void RenderContext::ApplyEffect( Texture* dst, Texture* src, Material* mat )
{
	m_effectCamera->SetColorTarget( dst );
	BeginCamera( *m_effectCamera );
	BindMaterial( mat );
	BindTexture( src );
	SetBlendMode( eBlendMode::BLEND_OPAQUE );
	m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_context->Draw( 3, 0 );
	EndCamera( *m_effectCamera );
}

void RenderContext::StartEffect( Texture* dst, Texture* src, Shader* shader )
{
	m_effectCamera->SetColorTarget( dst );
	BeginCamera( *m_effectCamera );
	BindShader( shader );
	BindTexture( src );
	SetBlendMode( eBlendMode::BLEND_OPAQUE );
}

void RenderContext::EndEffect()
{
	m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_context->Draw( 3, 0 );
	EndCamera( *m_effectCamera );
}

void RenderContext::CreateBlendState()
{
	D3D11_BLEND_DESC alphaDesc;
	alphaDesc.AlphaToCoverageEnable = false;
	alphaDesc.IndependentBlendEnable = false;
	alphaDesc.RenderTarget[0].BlendEnable = true;
	alphaDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	alphaDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	alphaDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	alphaDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	alphaDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	alphaDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	alphaDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	
	m_device->CreateBlendState( &alphaDesc, &m_alphaBlendState );
	
	D3D11_BLEND_DESC additiveDesc;
	additiveDesc.AlphaToCoverageEnable = false;
	additiveDesc.IndependentBlendEnable = false;
	additiveDesc.RenderTarget[0].BlendEnable = true;
	additiveDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	additiveDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	additiveDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	additiveDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	additiveDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	additiveDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	additiveDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	
	m_device->CreateBlendState( &additiveDesc, &m_additiveBlendState );
	
	D3D11_BLEND_DESC opaqueDesc;
	opaqueDesc.AlphaToCoverageEnable = false;
	opaqueDesc.IndependentBlendEnable = false;
	opaqueDesc.RenderTarget[0].BlendEnable = false;
	opaqueDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	opaqueDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	opaqueDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	opaqueDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	opaqueDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	opaqueDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	opaqueDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	
	m_device->CreateBlendState( &opaqueDesc, &m_opaqeBlendState );
}

void RenderContext::CreateDefaultRasterState()
{
	D3D11_RASTERIZER_DESC desc;

	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_NONE;
	desc.FrontCounterClockwise = TRUE; // the only reason we're doing this; 
	desc.DepthBias = 0U;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;

	m_device->CreateRasterizerState( &desc, &m_currentRasterState );
}

void RenderContext::SetBlendMode( eBlendMode blendMode )
{
	float const zeroes[] = { 0.f, 0.f, 0.f, 0.f };
	
	switch( blendMode )
	{
	case eBlendMode::BLEND_ALPHA:			m_context->OMSetBlendState( m_alphaBlendState, zeroes, (UINT)~0 );		break;
	case eBlendMode::BLEND_ADDITIVE:		m_context->OMSetBlendState( m_additiveBlendState, zeroes, (UINT)~0 );	break;
	case eBlendMode::BLEND_OPAQUE:			m_context->OMSetBlendState( m_opaqeBlendState, zeroes, (UINT)~0 );		break;
	default:	ERROR_AND_DIE( "UNKNOWN BLEND MODE" );
	}
}

void RenderContext::SetModelMatrix( const Mat44& mat )
{
	model_matrix model;
	model.matrix = mat;

	m_modelUBO->Update( &model, sizeof( model ), sizeof( model ) );
	BindUniformBuffer( 2, m_modelUBO );
}

void RenderContext::SetModelTint( Rgba8 tint )
{
	model_tint model;
	model.tint_color[0] = tint.r / 255.f;
	model.tint_color[1] = tint.g / 255.f;
	model.tint_color[2] = tint.b / 255.f;
	model.tint_color[3] = tint.a / 255.f;

	m_modelUBO->Update( &model, sizeof( model ), sizeof( model ) );
	BindUniformBuffer( 3, m_modelUBO );
}

void RenderContext::SetCullMode( eCullMode cullMode )
{
	D3D11_RASTERIZER_DESC desc;
	m_currentRasterState->GetDesc( &desc );
	switch( cullMode )
	{
	case eCullMode::CULL_NONE:	desc.CullMode = D3D11_CULL_NONE;  break;
	case eCullMode::CULL_FRONT:	desc.CullMode = D3D11_CULL_FRONT; break;
	case eCullMode::CULL_BACK:	desc.CullMode = D3D11_CULL_BACK;  break;
	default:												  break;
	}
	DX_SAFE_RELEASE( m_currentRasterState );
	m_device->CreateRasterizerState( &desc, &m_currentRasterState );
	BindShader( m_currentShader );
}

void RenderContext::SetFillMode( eFillMode fillMode )
{
	D3D11_RASTERIZER_DESC desc;
	m_currentRasterState->GetDesc( &desc );
	switch( fillMode )
	{
	case eFillMode::FILL_WIREFRAME:	desc.FillMode = D3D11_FILL_WIREFRAME;	break;
	case eFillMode::FILL_SOLID:		desc.FillMode = D3D11_FILL_SOLID;		break;
	default:															break;
	}
	DX_SAFE_RELEASE( m_currentRasterState );
	m_device->CreateRasterizerState( &desc, &m_currentRasterState );
	BindShader( m_currentShader );
}

void RenderContext::SetFrontFaceWindOrder( eWindingOrder windingOrder )
{
	D3D11_RASTERIZER_DESC desc;
	m_currentRasterState->GetDesc( &desc );
	desc.CullMode = D3D11_CULL_BACK;
	if ( eWindingOrder::WIND_CLOCKWISE == windingOrder )
	{
		desc.FrontCounterClockwise = false;
	}
	else if ( eWindingOrder::WIND_COUNTERCLOCKWISE == windingOrder )
	{
		desc.FrontCounterClockwise = true;
	}

	DX_SAFE_RELEASE( m_currentRasterState );
	m_device->CreateRasterizerState( &desc, &m_currentRasterState );
	BindShader( m_currentShader );
}

void RenderContext::EnableDepth( eCompareOp op, bool write )
{
	//ASSERT_OR_DIE( m_currentCamera != nullptr, "Only call between BeginCamera and EndCamera" );

	// ID3D11DepthStencilState* m_depthStencilState = nullptr; 
	if( m_depthStencilState != nullptr ) {
		DX_SAFE_RELEASE( m_depthStencilState );
	}

	D3D11_DEPTH_STENCIL_DESC desc;
	memset( &desc, 0, sizeof( desc ) ); // set everything to 0

	desc.DepthEnable = TRUE;
	desc.DepthWriteMask = write ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	switch( op )
	{
	case eCompareOp::COMPARISON_NEVER:			desc.DepthFunc = D3D11_COMPARISON_NEVER;			break;
	case eCompareOp::COMPARISON_LESS:			desc.DepthFunc = D3D11_COMPARISON_LESS;				break;
	case eCompareOp::COMPARISON_EQUAL:			desc.DepthFunc = D3D11_COMPARISON_EQUAL;			break;
	case eCompareOp::COMPARISON_LESS_EQUAL:		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;		break;
	case eCompareOp::COMPARISON_GREATER:		desc.DepthFunc = D3D11_COMPARISON_GREATER;			break;
	case eCompareOp::COMPARISON_NOT_EQUAL:		desc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;		break;
	case eCompareOp::COMPARISON_GREATER_EQUAL:	desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;	break;
	case eCompareOp::COMPARISON_ALWAYS:			desc.DepthFunc = D3D11_COMPARISON_ALWAYS;			break;
	default:
		break;
	}

	m_device->CreateDepthStencilState( &desc, &m_depthStencilState );
	m_context->OMSetDepthStencilState( m_depthStencilState, 0 );
}

void RenderContext::DisableDepth()
{
	EnableDepth( eCompareOp::COMPARISON_ALWAYS, false );
}

BitmapFont* RenderContext::CreateBitmapFontFromFile( const char* filePath )
{
	g_theConsole->PrintString( Rgba8::RED, "Loaded BitmapFont..." );
	std::string path = Stringf( "%s.png", filePath );
	Texture* fontTexture = CreateOrGetTextureFromFile( path.c_str() );
	g_theConsole->PrintString( Rgba8::GREEN, "BitmapFont loading succeed!" );
	BitmapFont* newBitFornt = new BitmapFont( path.c_str(), fontTexture );
	m_loadedFonts.push_back( newBitFornt );
	return newBitFornt;
}

BitmapFont* RenderContext::CreateBitmapFontFromFile( const char* filePath, const IntVec2& simpleGridLayout )
{
	g_theConsole->PrintString( Rgba8::RED, "Loaded BitmapFont..." );
	std::string path = Stringf( "%s.png", filePath );
	Texture* fontTexture = CreateOrGetTextureFromFile( path.c_str() );
	g_theConsole->PrintString( Rgba8::GREEN, "BitmapFont loading succeed!" );
	BitmapFont* newBitFornt = new BitmapFont( path.c_str(), fontTexture, simpleGridLayout );
	m_loadedFonts.push_back( newBitFornt );
	return newBitFornt;
}

BitmapFont* RenderContext::CreateOrGetBitmapFont( const char* bitmapFontFilePathNoExtension )
{
	for( int index = 0; index < m_loadedFonts.size(); index++ )
	{
		if( m_loadedFonts[index]->m_fontName == bitmapFontFilePathNoExtension )
		{
			return m_loadedFonts[index];
		}
	}
	return CreateBitmapFontFromFile( bitmapFontFilePathNoExtension );
}

BitmapFont* RenderContext::CreateOrGetBitmapFont( const char* bitmapFontFilePathNoExtension, const IntVec2& simpleGridLayout )
{
	for( int index = 0; index < m_loadedFonts.size(); index++ )
	{
		if( m_loadedFonts[index]->m_fontName == bitmapFontFilePathNoExtension )
		{
			return m_loadedFonts[index];
		}
	}
	return CreateBitmapFontFromFile( bitmapFontFilePathNoExtension, simpleGridLayout );
}

Texture* RenderContext::CreateRenderTarget( const IntVec2& texelSize )
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = texelSize.x;
	desc.Height = texelSize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1; // MSAA
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;


	ID3D11Texture2D* texHandle = nullptr;
	m_device->CreateTexture2D( &desc, nullptr, &texHandle );

	Texture* newTexure = new Texture( this, texHandle );

	return newTexure;
}

ShaderState* RenderContext::GetShaderStateFromName( const std::string& name )
{
	return ShaderState::GetDefinitions(name);
}

Material* RenderContext::GetMaterialFromName( const std::string& name )
{
	return Material::GetDefinitions(name);
}

Texture* RenderContext::GetBackBuffer() const
{
	return m_swapchain->m_backbuffer;
}

Texture* RenderContext::AcquireRenderTargetMatching( Texture* tex )
{
	if ( tex == nullptr )
	{
		return nullptr;
	}

	IntVec2 size = tex->GetDimensions();

	for( int i = 0; i < (int)m_renderTargetPool.size(); ++i )
	{
		Texture* rt = m_renderTargetPool[i];
		if( rt->GetDimensions() == size )
		{
			// fast remove at index
			m_renderTargetPool[i] = m_renderTargetPool[m_renderTargetPool.size() - 1];
			m_renderTargetPool.pop_back();

			// return the object from the pool
			return rt;
		}
	}

	// nothing in the pool that matches - make a new one
	++m_totalRenderTargetMade;
	Texture* newRenderTarget = CreateRenderTarget( size );
	return newRenderTarget;
}

void RenderContext::ReleaseRenderTarget( Texture* tex )
{
	if ( tex )
	{
		m_renderTargetPool.push_back( tex ); 
	}
}

void RenderContext::CopyTexture( Texture* dst, Texture* src )
{
	m_context->CopyResource( dst->GetHandle(), src->GetHandle() );
}

void RenderContext::UpdateLayoutIfNeeded()
{
	if( (m_previouslyBoundLayout != m_currentLayout) || (m_shaderHasChanged) ) {
		ID3D11InputLayout* layout = m_currentShader->GetOrCreateInputLayout( m_currentLayout );
		m_context->IASetInputLayout( layout );

		m_previouslyBoundLayout = m_currentLayout;
		m_shaderHasChanged = false;
	}
}

Texture* RenderContext::CreateTextureFromFile( const char* imageFilePath )
{
	//unsigned int textureID = 0;
	int imageTexelSizeX = 0; // This will be filled in for us to indicate image width
	int imageTexelSizeY = 0; // This will be filled in for us to indicate image height
	int numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 4; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)
	
	// Load (and decompress) the image RGB(A) bytes from a file on disk into a memory buffer (array of bytes)
	stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* imageData = stbi_load( imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested );

	// Check if the load was successful
	GUARANTEE_OR_DIE( imageData, Stringf( "Failed to load image \"%s\"", imageFilePath ) );
	GUARANTEE_OR_DIE( numComponents == 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, numComponents, imageTexelSizeX, imageTexelSizeY ) );
	
	// describe the texture
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = imageTexelSizeX;
	desc.Height = imageTexelSizeY;
	desc.MipLevels = 1; // setting to 0 means there's a full chain (or can generate a full chain)
	desc.ArraySize = 1; // only one texture
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // for now assume 4 channel RGBA
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE; // loaded from image - probably not changing
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // only allowing rendertarget for mipmap generation
	desc.CPUAccessFlags = 0; // Determines how I can access this resource CPU side 
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem = imageData;
	initialData.SysMemPitch = imageTexelSizeX * 4;
	initialData.SysMemSlicePitch = 0;

	// DirectX Creation
	ID3D11Texture2D* texHandle = nullptr;
	m_device->CreateTexture2D( &desc, &initialData, &texHandle );

	//// Free the raw image texel data now that we've sent a copy of it down to the GPU to be stored in video memory
	stbi_image_free( imageData );
	Texture* tex = new Texture( imageFilePath, this, texHandle );
	m_textureList.push_back( tex );
	g_theConsole->PrintString( Rgba8::GREEN, Stringf( "Texture %s loading succeed!", imageFilePath ) );
	
	return tex;
}