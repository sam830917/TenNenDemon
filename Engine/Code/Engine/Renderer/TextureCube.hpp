#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Image.hpp"
#include <vector>
#include <string>

enum eTextureCubeSide : uint
{
	TEXCUBE_RIGHT,
	TEXCUBE_LEFT,
	TEXCUBE_TOP,
	TEXCUBE_BOTTOM,
	TEXCUBE_FRONT, // -z
	TEXCUBE_BACK,  // +z

	TEXCUBE_SIDE_COUNT,
};

class RenderContext;
class TextureView;

struct ID3D11Texture2D;

class TextureCube
{
public:
	TextureCube( RenderContext* ctx, Image* src );
	TextureCube( RenderContext* ctx, Image& src );
	~TextureCube();

	TextureView* GetOrCreateShaderResourceView();

private:
	IntVec2		m_dimensions;

	RenderContext* m_owner = nullptr;
	ID3D11Texture2D* m_handle = nullptr;
	TextureView* m_shaderResourceView = nullptr;
};