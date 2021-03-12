#include "Engine/Renderer/Transform.hpp"
#include "Engine/Math/MathUtils.hpp"

void Transform::SetPosition( Vec3 pos )
{
	m_position = pos;
}

void Transform::Translate( Vec3 translation )
{
	m_position += translation;
}

void Transform::SetRotationFromPitchRollYawDegrees( float pitch, float roll, float yaw )
{
	pitch = Clamp( pitch, -180.f, 180.f );
	if( pitch == -180.f )
	{
		pitch = 180.f;
	}
	else if( pitch == 180.f )
	{
		pitch = -180.f;
	}
	roll = Clamp( roll, -180.f, 180.f );
	if( roll == -180.f )
	{
		roll = 180.f;
	}
	else if( roll == 180.f )
	{
		roll = -180.f;
	}
	yaw = Clamp( yaw, -180.f, 180.f );
	if( yaw == -180.f )
	{
		yaw = 180.f;
	}
	else if( yaw == 180.f )
	{
		yaw = -180.f;
	}
	m_pitch = pitch;
	m_roll = roll;
	m_yaw = yaw;
}

void Transform::AddRotationFromPitchRollYawDegrees( float pitch, float roll, float yaw )
{
	float newPitch = pitch + pitch;
	float newRoll = roll + roll;
	float newYaw = yaw + yaw;

	SetRotationFromPitchRollYawDegrees( newPitch, newRoll, newYaw );
}

Mat44 Transform::GetMatrix() const
{
	Mat44 mat = Mat44::IDENTITY;
	mat.Translate3D( m_position );
	mat.TransformBy( GetRotationMatrix() );
	mat.ScaleNonUniform3D( m_scale );
	return mat;
}

Mat44 Transform::GetRotationMatrix() const
{
	Vec3 rotationWithDegrees = Vec3( m_roll, m_pitch, m_yaw );

	Mat44 mat = Mat44::IDENTITY;
	mat.RotateZDegrees( rotationWithDegrees.z );
	mat.RotateYDegrees( rotationWithDegrees.y );
	mat.RotateXDegrees( rotationWithDegrees.x );
	return mat;
}

const Mat44 Transform::GetBasisTransformMatrix( eWorldBasis basis )
{
	Mat44 mat = Mat44::IDENTITY;
	switch( basis )
	{
	case WORLD_BASIS_XYZ:
		break;
	case WORLD_BASIS_XZnY:
		mat.SetBasisVectors3D
		(
			Vec3(1.f, 0.f, 0.f),
			Vec3(0.f, 0.f, 1.f),
			Vec3(0.f, -1.f, 0.f)
		);
		break;
	case WORLD_BASIS_nZnXY:
		mat.SetBasisVectors3D
		(
			Vec3( 0.f, 0.f, -1.f ),
			Vec3( -1.f, 0.f, 0.f ),
			Vec3( 0.f, 1.f, 0.f )
		);
		break;
	default:
		break;
	}
	return mat;
}
