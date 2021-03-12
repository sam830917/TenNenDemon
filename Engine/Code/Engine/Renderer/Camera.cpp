#include "Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/MathUtils.hpp"

Camera::Camera()
{
	SetClearMode( CLEAR_COLOR_BIT );
}

Camera::Camera( Vec2 size )
{
	SetClearMode( CLEAR_COLOR_BIT );
	SetOutputSize( size );
}

Camera::~Camera()
{
}

void Camera::SetOutputSize( Vec2 size )
{
	m_outputSize = size;
	m_aspectRatio = m_outputSize.x / m_outputSize.y;
}

void Camera::SetProjectionOrthographic( float height, float nearZ, float farZ )
{
	m_outputSize = Vec2( height * GetAspectRatio(), height );
	m_nearZ = nearZ;
	m_farZ = farZ;
	m_orthoBottomLeft = Vec2(m_transform.m_position.x, m_transform.m_position.y) - (m_outputSize * 0.5f);
	m_orthoTopRight = Vec2(m_transform.m_position.x, m_transform.m_position.y) + (m_outputSize * 0.5f);
	m_projection = Mat44::CreateOrthographicProjection( Vec3( m_orthoBottomLeft, 0.f ), Vec3( m_orthoTopRight, 1.f ) );
}

void Camera::SetProjectionOrthographic( Vec2 min, Vec2 max, float nearZ, float farZ )
{
	m_outputSize = max - min;
	m_nearZ = nearZ;
	m_farZ = farZ;
	m_orthoBottomLeft = min;
	m_orthoTopRight = max;
	m_projection = Mat44::CreateOrthographicProjection( Vec3( min, 0.f ), Vec3( max, 1.f ) );
}

void Camera::SetProjectionPerspective( float fov, float nearZ, float farZ )
{
	// float fov is the field of view you want
	// float farZ and nearZ are the depth range you want to see
	// -> do **not** span zero here.

	// Goal is to...
	// - setup a default "depth" where (1, 1) == (1, 1) after projection (1 / tan(fov * .5f))
	// - map z to w, so the z divide happens (we go along negative, so we want a negative w-divide)
	// - map nearZ to 0, farZ to -farZ, since a -Z divide will happen
	//   and this will result in mapping nearZ to 0, and farZ to 1 in NDC space 
	//   -> ((z - nz) / (fz - nz)) * fz + 0
	//   -> fz / (fz - nz) * z      + (-fz * nz) / (fz - nz)
	float height = 1.0f / TanDegrees( fov * 0.5f ); // how far away are we for the perspective point to be "one up" from our forward line. 
	float zrange = farZ - nearZ;
	float q = 1.0f / zrange;

	float proj[] =
	{
		height / m_aspectRatio,		0,			0,					0,
		0,							height,		0,					0,
		0,							0,			-farZ * q,			-1,
		0,							0,			nearZ * farZ * q,	0
	};

	m_projection = Mat44( proj );
}

Vec2 Camera::ClientToWorldPosition( Vec2 clientPos, float ndcZ ) const
{
	UNUSED(ndcZ);
	Vec2 localPos = m_outputSize * clientPos;
	return localPos + GetOrthoMin().GetXY();
}

Vec3 Camera::ClientToWorld( Vec2 client, float ndcZ ) const
{
	Vec3 ndc = RangeMap( Vec3( 0, 0, 0 ), Vec3( 1, 1, 1 ), Vec3( -1, -1, 0 ), Vec3( 1, 1, 1 ), Vec3( client, ndcZ ) );

	Mat44 proj = GetProjectionMatrix();
	Mat44 worldToClip = proj;
	worldToClip.TransformBy( GetViewMatrix() );

	Mat44 clipToWorld = worldToClip.GetInvert();
	Vec4 worldHomogeneous = clipToWorld.TransformHomogeneousPoint3D( Vec4( ndc, 1 ) );
	Vec3 worldPos = worldHomogeneous.GetXYZ() / worldHomogeneous.w;

	return worldPos;
}

Vec3 Camera::WorldToClient( Vec3 worldPos ) const
{
	Mat44 model = m_transform.GetMatrix();

	return worldPos;
}

void Camera::SetPosition( const Vec3& position )
{
	m_orthoBottomLeft = Vec2( m_transform.m_position.x, m_transform.m_position.y ) - (m_outputSize * 0.5f);
	m_orthoTopRight = Vec2( m_transform.m_position.x, m_transform.m_position.y ) + (m_outputSize * 0.5f);
	m_transform.SetPosition( position );
}

void Camera::Translate( const Vec3& translation )
{
	m_transform.Translate( translation );
}

void Camera::Translate2D( const Vec2& translation )
{
	m_orthoBottomLeft += translation;
	m_orthoTopRight += translation;
}

void Camera::SetClearMode( eCameraClearFlags clearFlags, Rgba8 color, float depth, uint stencil )
{
	m_clearMode = clearFlags;
	m_clearColor = color;
	m_clearDepth = depth;
	m_clearStencil = stencil;
}

void Camera::SetDepthStencilBuffer( Texture* tex )
{
	m_depthStencilTarget = tex;
}

Vec3 Camera::GetOrthoMax() const
{
	return ClientToWorld( Vec2( 1, 1 ), 0 );
}

Vec3 Camera::GetOrthoMin() const
{
	return ClientToWorld( Vec2( 0, 0 ), 0 );
}

Mat44 Camera::GetViewMatrix() const
{
	Mat44 cameraView = Mat44::IDENTITY;
	switch( m_worldBasis )
	{
	case WORLD_BASIS_XFORWARD_YLEFT_ZUP:
		cameraView = Transform::GetBasisTransformMatrix( WORLD_BASIS_nZnXY );
		cameraView.RotateXDegrees( -m_transform.GetRotationRoll() );
		cameraView.RotateYDegrees( -m_transform.GetRotationPitch() );
		cameraView.RotateZDegrees( -m_transform.GetRotationYaw() );
		cameraView.Translate3D( -m_transform.m_position );

		return cameraView;
	case WORLD_BASIS_XRIGHT_YUP_ZBACK:
	default:
		cameraView = m_transform.GetMatrix();
		return cameraView.GetInvertOrthoNormal();
	}
}

Texture* Camera::GetColorTarget() const
{
	if ( m_colorTargets.size() == 0 )
	{
		return nullptr;
	}
	return m_colorTargets[0];
}

Texture* Camera::GetColorTarget( int index ) const
{
	if ( index < GetColorTargetCount() )
	{
		return m_colorTargets[index];
	}
	return nullptr;
}

void Camera::UpdateUBO() const
{
	camera_data_t frameData;
	frameData.projection = m_projection;
	frameData.view = GetViewMatrix();
	frameData.position = m_transform.m_position;

	m_cameraUBO->Update( &frameData, sizeof( frameData ), sizeof( frameData ) );
}

RenderBuffer* Camera::GetAndUpdateUBO() const
{
	if ( m_cameraUBO == nullptr )
	{
		return nullptr;
	}
	UpdateUBO();
	return m_cameraUBO;
}

void Camera::InitialUBO( RenderContext* ctx )
{
	m_cameraUBO = new RenderBuffer( ctx, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	m_owner = ctx;
}

void Camera::ClearupUBO()
{
	delete m_cameraUBO;
	m_cameraUBO = nullptr;
}

void Camera::SetColorTarget( Texture* tex )
{
	SetColorTarget( 0, tex );
}

void Camera::SetColorTarget( int index, Texture* target )
{
	if ( index >= (int) m_colorTargets.size() )
	{
		m_colorTargets.resize( index + 1 );
	}

	m_colorTargets[index] = target;
}
