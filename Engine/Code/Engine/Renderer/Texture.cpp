#include "Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/D3D11Common.hpp"

Texture::Texture( const char* filePath, RenderContext* ctx, ID3D11Texture2D* handle )
	: Texture(ctx, handle)
{
	m_imageFilePath = filePath;
}

Texture::Texture( const Rgba8& color, RenderContext* ctx )
{
	unsigned char imageData[4] = { color.r, color.g, color.b, color.a };

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = 1;
	desc.Height = 1;
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
	initialData.pSysMem = &imageData;
	initialData.SysMemPitch = 4;
	initialData.SysMemSlicePitch = 0;

	ID3D11Texture2D* texHandle = nullptr;
	ctx->m_device->CreateTexture2D( &desc, &initialData, &texHandle );

	m_owner = ctx;
	m_handle = texHandle;
	m_dimensions = IntVec2( 1, 1 );
}

Texture::Texture( RenderContext* ctx, ID3D11Texture2D* handle )
	:m_owner(ctx),
	m_handle(handle)
{
	D3D11_TEXTURE2D_DESC desc;
	handle->GetDesc( &desc );
	m_dimensions = IntVec2( desc.Width, desc.Height );
}

Texture::~Texture()
{
	delete m_renderTargetView;
	m_renderTargetView = nullptr;
	delete m_shaderResourceView;
	m_shaderResourceView = nullptr;
	delete m_depthStencilView;
	m_depthStencilView = nullptr;

	DX_SAFE_RELEASE(m_handle);
}

TextureView* Texture::GetOrCreateRenderTargetView()
{
	if (m_renderTargetView)
	{
		return m_renderTargetView;
	}

	ID3D11Device* dev = m_owner->m_device;
	ID3D11RenderTargetView* srv = nullptr;
	dev->CreateRenderTargetView( m_handle, nullptr, &srv );

	if ( srv != nullptr )
	{
		m_renderTargetView = new TextureView();
		m_renderTargetView->m_rtv = srv;
	}

	return m_renderTargetView;
}

TextureView* Texture::GetOrCreateShaderResourceView()
{
	if ( m_shaderResourceView != nullptr )
	{
		return m_shaderResourceView;
	}

	ID3D11Device* dev = m_owner->m_device;
	ID3D11ShaderResourceView* srv = nullptr;

	dev->CreateShaderResourceView( m_handle, nullptr, &srv );
	if ( srv != nullptr )
	{
		m_shaderResourceView = new TextureView();
		m_shaderResourceView->m_srv = srv;
	}

	return m_shaderResourceView;
}

TextureView* Texture::GetOrCreateShaderResourceViewCube()
{
	if( m_shaderResourceView != nullptr )
	{
		return m_shaderResourceView;
	}

	ID3D11Device* dev = m_owner->m_device;
	ID3D11ShaderResourceView* srv = nullptr;

	CD3D11_SHADER_RESOURCE_VIEW_DESC view_desc;
	ZeroMemory( &view_desc, sizeof( view_desc ) );

	view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	view_desc.TextureCube.MostDetailedMip = 0;
	view_desc.TextureCube.MipLevels = 1;

	dev->CreateShaderResourceView( m_handle, &view_desc, &srv );
	if( srv != nullptr )
	{
		m_shaderResourceView = new TextureView();
		m_shaderResourceView->m_srv = srv;
	}

	return m_shaderResourceView;
}

TextureView* Texture::GetOrCreateDepthStencilView()
{
	if( m_depthStencilView != nullptr )
	{
		return m_depthStencilView;
	}

	// Create Depth Stencil Target View
	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	ZeroMemory( &dsv_desc, sizeof( dsv_desc ) );
	dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
	dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Texture2D.MipSlice = 0;

	ID3D11DepthStencilView* dsv = nullptr;
	ID3D11Device* device = m_owner->m_device;
	device->CreateDepthStencilView( m_handle, &dsv_desc, &dsv );

	if ( dsv != nullptr )
	{
		m_depthStencilView = new TextureView();
		m_depthStencilView->m_dsv = dsv;
	}

	return m_depthStencilView;
}

Texture* Texture::CreateDepthStencilBuffer( RenderContext* ctx, uint widht, uint height )
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = widht;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_D32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	// Actually create it
	ID3D11Texture2D* tex2D = nullptr;
	ID3D11Device* device = ctx->m_device;
	HRESULT hr = device->CreateTexture2D( &texDesc, nullptr, &tex2D );

	if( !SUCCEEDED( hr ) ) {
		return nullptr;
	}
	Texture* tex = new Texture( ctx, tex2D );

	return tex;
}

float Texture::GetAspect() const
{
	return (float) m_dimensions.x / (float) m_dimensions.y;
}
