#include "Engine/Renderer/TextureCube.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/D3D11Common.hpp"

#include "ThirdParty/stb/stb_image.h"

TextureCube::TextureCube( RenderContext* ctx, Image* src )
{
	m_owner = ctx;
	IntVec2 dimensions = src[0].GetDimensions();
	uint width = dimensions.x;
	uint height = dimensions.y;
	m_dimensions = IntVec2( width, height );

	D3D11_TEXTURE2D_DESC cube_desc;
	ZeroMemory( &cube_desc, sizeof( cube_desc ) );
	cube_desc.MipLevels = 1;
	cube_desc.ArraySize = TEXCUBE_SIDE_COUNT;
	cube_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	cube_desc.Usage = D3D11_USAGE_IMMUTABLE;
	cube_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	cube_desc.CPUAccessFlags = 0;
	cube_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	cube_desc.Width = width;
	cube_desc.Height = width;

	// multisampling
	cube_desc.SampleDesc.Count = 1;
	cube_desc.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA data[6];
	D3D11_SUBRESOURCE_DATA* pdata = nullptr;
	pdata = data;
	ZeroMemory( &data, sizeof( data ) );

	for( int i = 0; i < TEXCUBE_SIDE_COUNT; ++i )
	{
		data[i].pSysMem = src[i].GetTexelsBuffer();
		data[i].SysMemPitch = width * 4;
	}
	ID3D11Texture2D* texHandle = nullptr;
	ctx->m_device->CreateTexture2D( &cube_desc, pdata, &texHandle );
	m_handle = texHandle;
}

TextureCube::TextureCube( RenderContext* ctx, Image& src )
{
	m_owner = ctx;
	IntVec2 dimensions = src.GetDimensions();
	uint width = dimensions.x / 4;
	uint height = dimensions.y / 3 ;
	m_dimensions = IntVec2( width, height );

	D3D11_TEXTURE2D_DESC cube_desc;
	ZeroMemory( &cube_desc, sizeof( cube_desc ) );
	cube_desc.MipLevels = 1;
	cube_desc.ArraySize = TEXCUBE_SIDE_COUNT;
	cube_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	cube_desc.Usage = D3D11_USAGE_IMMUTABLE;
	cube_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	cube_desc.CPUAccessFlags = 0;
	cube_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	cube_desc.Width = width;
	cube_desc.Height = width;

	// multisampling
	cube_desc.SampleDesc.Count = 1;
	cube_desc.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA data[6];
	D3D11_SUBRESOURCE_DATA* pdata = nullptr;
	pdata = data;
	ZeroMemory( &data, sizeof( data ) );
	int pitch = width * 4;
	int total_pitch = 4 * pitch;
	int row = width * total_pitch;
	unsigned char* start = src.GetTexelsBuffer();

	int offsets[] =
	{
		(1 * row) + (2 * pitch),   // right
		(1 * row) + (0 * pitch),   // left
		(0 * row) + (1 * pitch),   // top
		(2 * row) + (1 * pitch),   // bottom
		(1 * row) + (1 * pitch),   // front
		(1 * row) + (3 * pitch)    // back
	};

	for( int i = 0; i < TEXCUBE_SIDE_COUNT; ++i )
	{
		data[i].pSysMem = start + offsets[i];
		data[i].SysMemPitch = total_pitch;
	}
	ID3D11Texture2D* texHandle = nullptr;
	ctx->m_device->CreateTexture2D( &cube_desc, pdata, &texHandle );
	m_handle = texHandle;
}

TextureCube::~TextureCube()
{
	delete m_shaderResourceView;
	m_shaderResourceView = nullptr;

	DX_SAFE_RELEASE( m_handle );
}

TextureView* TextureCube::GetOrCreateShaderResourceView()
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
