#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <string>

class RenderContext;
class TextureView;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;

class Texture
{
public:
	explicit Texture( const char* filePath, RenderContext* ctx, ID3D11Texture2D* handle );
	explicit Texture( const Rgba8& color, RenderContext* ctx );
	explicit Texture( RenderContext* ctx, ID3D11Texture2D* handle );
	~Texture();

	TextureView* GetOrCreateRenderTargetView();
	TextureView* GetOrCreateShaderResourceView();
	TextureView* GetOrCreateShaderResourceViewCube();
	TextureView* GetOrCreateDepthStencilView();

	IntVec2				GetDimensions() const { return m_dimensions; }
	std::string const&	GetFilePath() const { return m_imageFilePath; }
	float				GetAspect() const;
	RenderContext*		GetRenderContext() const { return m_owner; }
	ID3D11Texture2D*	GetHandle() const { return m_handle; }

	static Texture* CreateDepthStencilBuffer( RenderContext* ctx, uint widht, uint height );

private:
	RenderContext* m_owner = nullptr;
	ID3D11Texture2D* m_handle = nullptr;
	std::string m_imageFilePath;
	IntVec2		m_dimensions;

	TextureView*  m_renderTargetView = nullptr;
	TextureView*  m_shaderResourceView = nullptr;
	TextureView*  m_depthStencilView = nullptr;
};