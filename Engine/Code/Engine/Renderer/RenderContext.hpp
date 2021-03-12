#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/RenderCommon.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Polygon2.hpp"
#include "Engine/Platform/Window.hpp"
#include <vector>
#include <map>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11BlendState;
struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;

class Window;
class Shader;
class VertexBuffer;
class IndexBuffer;
class Sampler;
class GPUMesh;
class Material;
class TextureCube;

struct light_t
{
	Vec3 position;
	float pad00;

	float color[3];
	float intensity;

	Vec3 attenuation = Vec3( 0.f, 1.f, 0.f );
	float dot_inner_angle = -1.f;

	Vec3 direction = Vec3( 0.f, 0.f, 1.f );// direction light is point, default (0,0,1)
	float direction_factor = 0.f; // do we consider this a point light (0.0f) or a directional light (1.0f).  Determine how we calculate the incident light vector, default (0.0f)

	Vec3 spec_attenuation = Vec3( 0.f, 0.f, 1.f );
	float dot_outer_angle = -1.f;
};

struct frame_data_t
{
	float system_time;
	float system_delta_time;
	float padding00[2];
};

struct model_matrix
{
	Mat44 matrix;
};

struct model_tint
{
	float tint_color[4];
};

struct light_constants
{
	float ambient[4] = { 1.f, 1.f, 1.f, 0.f };

	light_t lights[LIGHT_NUMBER];

	float specularPower = 50.f;
	float specularFactor = 1.f;

	// fog
	float fog_near_distance;
	float fog_far_distance;

	float fog_near_color[3];
	float fogpad0;
	float fog_far_color[3];
	float fog_intensity = 0.f;
};

enum eBufferSlot
{
	UBO_FRAME_SLOT = 0,
	UBO_CAMERA_SLOT = 1,
};

class RenderContext
{
public:
	void Startup( Window* window );
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void ClearScreen( const Rgba8& clearColor );
	void BeginCamera( const Camera& camera );
	void EndCamera( const Camera& camera );
	void UpdateFrameTime( float deltaTime );
	void ClearDepth( Texture* depthStencilTexture, float depth, uint stencil );

	//------------------------------------------------------------------------------------------------------------------------
	//												Bind functions
	//------------------------------------------------------------------------------------------------------------------------
	void BindTexture( const Texture* texture );
	void BindTextureCube( const TextureCube* texture );
	void BindNormalTexture( const Texture* texture );
	void BindTextureOnSlot( uint slot, const Texture* texture );
	void BindSampler( Sampler* sampler );
	void BindSamplerOnSlot( uint slot, Sampler* sampler );
	void BindShader( Shader* shader );
	void BindShaderFromFile( char const* filename );
	void BindUniformBuffer( uint slot, RenderBuffer* ubo );
	void BindUniformBuffer( uint slot, const Camera* camera );
	void BindVertexBuffer( uint slot, VertexBuffer* vertexBuffer );
	void BindIndexBuffer( IndexBuffer* indexBuffer );
	void BindShaderStateFromName( const std::string& name );
	void BindShaderState( ShaderState* shaderState );
	void BindMaterial( Material* mat );
	void BindMaterialFromName( const std::string& name );
	void BindMaterialBuffer( RenderBuffer* buf );

	//------------------------------------------------------------------------------------------------------------------------
	//												Create or Get functions
	//------------------------------------------------------------------------------------------------------------------------
	Shader*		 CreateOrGetShader( char const* filename );
	Texture*	 CreateOrGetTextureFromFile( const char* imageFilePath );
	Texture*	 CreateTextureFromColor( Rgba8 color );
	BitmapFont*  CreateOrGetBitmapFont( const char* bitmapFontFilePathNoExtension );
	BitmapFont*  CreateOrGetBitmapFont( const char* bitmapFontFilePathNoExtension, const IntVec2& simpleGridLayout );
	Texture*	 CreateRenderTarget( const IntVec2& texelSize );

	ShaderState* GetShaderStateFromName( const std::string& name );
	Material*	 GetMaterialFromName( const std::string& name );
	Texture*	 GetDefaultBackbuffer() { return m_defultBackBuffer; }
	Texture*	 GetBackBuffer() const;
	Texture*	 AcquireRenderTargetMatching( Texture* tex );
	void		 ReleaseRenderTarget( Texture* tex );
	void		 CopyTexture( Texture* dst, Texture* src );

	//------------------------------------------------------------------------------------------------------------------------
	//												Set functions
	//------------------------------------------------------------------------------------------------------------------------
	void SetBlendMode( eBlendMode blendMode );
	void SetModelMatrix( const Mat44& mat );
	void SetModelTint( Rgba8 tint );
	void SetCullMode( eCullMode cullMode );
	void SetFillMode( eFillMode fillMode );
	void SetFrontFaceWindOrder( eWindingOrder windingOrder = eWindingOrder::WIND_COUNTERCLOCKWISE );
	void SetAmbientColor( const Rgba8& color );
	void SetAmbientIntensity( float intensity );
	void SetAmbientLight( const Rgba8& color, float intensity );
	void SetSpecularFactor( float specularFactor );
	void SetSpecularPower( float specularPower );

	//------------------------------------------------------------------------------------------------------------------------
	//												Enable functions
	//------------------------------------------------------------------------------------------------------------------------
	bool IsDrawing() { return m_isDrawing; }
	void EnableDepth( eCompareOp op, bool write );
	void DisableDepth();

	// for now, assume idx is 0, we only have one light
	void EnableLight( uint idx, light_t const& lightInfo );
	void DisableLight( uint idx );

	void EnableFog( float nearFog, float farFog, const Rgba8& nearFogColor, const Rgba8& farFogColor );
	void DisableFog();

	void ApplyEffect( Texture* dst, Texture* src, Material* mat );
	void StartEffect( Texture* dst, Texture* src, Shader* shader );
	void EndEffect();

	//------------------------------------------------------------------------------------------------------------------------
	//												Draw functions
	//------------------------------------------------------------------------------------------------------------------------
	void Draw( int numVertexes, int vertexOffset = 0 );
	void DrawIndexed( int indexCount, int vertexOffset, int baseVertexOffset );
	void DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void DrawVertexArray( const std::vector<Vertex_PCU> vertexes );
	void DrawLine( const Vec2& startVec2, const Vec2& endVec2, const Rgba8& color, float thickness );
	void DrawLinesFromPoints( int pointsCount, const Vec2* points, const Rgba8& color, float thickness );
	void DrawAABB2D( const AABB2& bounds, const Rgba8& tint );
	void DrawQuad( const Vec3& mins, const Vec3& maxs, const Rgba8& tint );
	void DrawAABB2D( const AABB2& bounds, const AABB2& spriteSheetUV, const Rgba8& tint );
	void DrawPolyGon2D( const Polygon2& polygon, const Rgba8& tint );
	void DrawCapsule2( const Vec2& boneStart, const Vec2& boneEnd, float radius, const Rgba8& color );
	void DrawRing( const Vec2& centerVec2, float radius, const Rgba8& color, float thickness );
	void DrawCircle( const Vec2& centerVec2, float radius, const Rgba8& color, float rotation, float renderDegrees = 360.f );
	void DrawText2D( const std::string& text, const Vec2& position, float size, BitmapFont* font, const Rgba8& color );
	void DrawMesh( GPUMesh* mesh );

private:
	void		UpdateLayoutIfNeeded();
	BitmapFont* CreateBitmapFontFromFile( const char* filePath );
	BitmapFont* CreateBitmapFontFromFile( const char* filePath, const IntVec2& simpleGridLayout );
	Texture*	CreateTextureFromFile( const char* imageFilePath );
	void		CreateBlendState();
	void		CreateDefaultRasterState();

private:
	std::vector<Texture*> m_textureList;
	std::vector<Shader*> m_shaderList;
	std::vector<BitmapFont*> m_loadedFonts;
	std::map<std::string, std::vector<Vertex_PCUTBN>> m_objList;

	std::vector<Texture*> m_renderTargetPool;
	int m_totalRenderTargetMade = 0; // determine if leaking, debug purpose

public:
	bool m_isDrawing = false;
	ID3D11Device*			m_device = nullptr;
	ID3D11Buffer*			m_lastVBO = nullptr;
	ID3D11DeviceContext*	m_context = nullptr; // Immediate context

	bool		m_shaderHasChanged = false;
	SwapChain*	m_swapchain = nullptr;
	Shader*		m_defaultShader = nullptr;
	Shader*		m_currentShader = nullptr;

	RenderBuffer* m_frameUBO = nullptr;
	RenderBuffer* m_modelUBO = nullptr;
	RenderBuffer* m_lightUBO = nullptr;

	Texture* m_texDefultColor = nullptr;
	Texture* m_texDefultNormal = nullptr;
	Sampler* m_defultSampler = nullptr;
	Texture* m_defultBackBuffer = nullptr;

	ID3D11BlendState*			m_alphaBlendState = nullptr;
	ID3D11BlendState*			m_additiveBlendState = nullptr;
	ID3D11BlendState*			m_opaqeBlendState = nullptr;
	ID3D11RasterizerState*		m_currentRasterState = nullptr;
	ID3D11DepthStencilState*	m_depthStencilState = nullptr;

	buffer_attribute_t const* m_currentLayout;
	buffer_attribute_t const* m_previouslyBoundLayout;
	light_constants m_lightConstants;

	GPUMesh* m_immediateMesh = nullptr;
	Camera* m_effectCamera = nullptr;

};