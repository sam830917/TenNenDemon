#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Transform.hpp"
#include <vector>

class RenderBuffer;
class RenderContext;
class Texture;

typedef unsigned int uint;
typedef uint eCameraClearFlags;

enum eCameraClearBitlag : unsigned int
{
	CLEAR_NONE,

	CLEAR_COLOR_BIT		= (1 << 0),
	CLEAR_DEPTH_BIT		= (1 << 1),
	CLEAR_STENCIL_BIT	= (1 << 2),
};

enum eWorldBasisConvention
{
	WORLD_BASIS_XRIGHT_YUP_ZBACK,
	WORLD_BASIS_XFORWARD_YLEFT_ZUP,
};

struct camera_data_t
{
	Mat44 projection;
	Mat44 view;

	Vec3 position;
	float padding00;
};

class Camera
{
public:
	Camera();
	Camera( Vec2 size );
	~Camera();

	Vec2 ClientToWorldPosition( Vec2 clientPos, float ndcZ ) const;
	Vec3 ClientToWorld( Vec2 client, float ndcZ = 0 ) const;
	Vec3 WorldToClient( Vec3 worldPos ) const;

	void Translate( const Vec3& translation);
	void Translate2D( const Vec2& translation );

	void SetOutputSize( Vec2 size );
	void SetPosition( const Vec3& position );
	void SetProjectionOrthographic( float height, float nearZ = -1.0f, float farZ = 1.0f );
	void SetProjectionOrthographic( Vec2 min, Vec2 max, float nearZ = -1.0f, float farZ = 1.0f );
	void SetProjectionPerspective( float fovDegrees, float nearZ, float farZ );
	void SetColorTarget( Texture* tex );
	void SetColorTarget( int index, Texture* target );
	void SetClearMode( eCameraClearFlags clearFlags, Rgba8 color = Rgba8::BLACK, float depth = 1.0f, uint stencil = 0 );
	void SetDepthStencilBuffer( Texture* tex );
	void SetWorldBasis( eWorldBasisConvention worldBasisConvention ) { m_worldBasis = worldBasisConvention; }

	Vec3 GetOrthoMax() const;
	Vec3 GetOrthoMin() const;
	Vec2 GetOrthoBottomLeft() const { return m_orthoBottomLeft; };
	Vec2 GetOrthoTopRight() const { return m_orthoTopRight; };

	eCameraClearFlags GetClearMode() const { return m_clearMode; }
	Vec2	GetCameraDimensions() const { return m_orthoTopRight - m_orthoBottomLeft; }
	float	GetAspectRatio() const { return m_aspectRatio; }
	Rgba8	GetClearColor() const { return m_clearColor; }
	float	GetClearDepth() const { return m_clearDepth; }
	uint	GetClearStencil() const { return m_clearStencil; }
	Mat44	GetProjectionMatrix() const { return m_projection; }
	Mat44	GetViewMatrix() const;

	Texture*		GetColorTarget() const;
	Texture*		GetColorTarget( int index ) const;
	int				GetColorTargetCount() const { return (int) m_colorTargets.size(); };
	Texture*		GetDepthStencilBuffer() const { return m_depthStencilTarget; }
	RenderBuffer*	GetUBO() const { return m_cameraUBO; }
	RenderBuffer*	GetAndUpdateUBO() const;

	void InitialUBO( RenderContext* ctx );
	void UpdateUBO() const;
	void ClearupUBO();

public:
	Transform m_transform;
	RenderContext* m_owner = nullptr;

	float m_aspectRatio = 0.f;
	float m_nearZ = 0;
	float m_farZ = 0;

	Mat44 m_projection;
	Mat44 m_view;
	Vec2 m_outputSize;
	Vec2 m_orthoBottomLeft;
	Vec2 m_orthoTopRight;

	eCameraClearFlags	m_clearMode		= 0;
	Rgba8				m_clearColor	= Rgba8::BLACK;
	float				m_clearDepth    = 1.0f;
	uint				m_clearStencil  = 0;

	std::vector<Texture*> m_colorTargets;
	Texture* m_depthStencilTarget = nullptr;
	eWorldBasisConvention m_worldBasis = WORLD_BASIS_XRIGHT_YUP_ZBACK;

	RenderBuffer* m_cameraUBO = nullptr;
};