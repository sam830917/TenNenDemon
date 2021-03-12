#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"

enum eWorldBasis
{
	WORLD_BASIS_XYZ,
	WORLD_BASIS_XZnY,
	WORLD_BASIS_nZnXY,
};

class Transform
{
public:
	void SetPosition( Vec3 pos );
	void Translate ( Vec3 translation );
	void SetRotationFromPitchRollYawDegrees( float pitch, float roll, float yaw );
	void AddRotationFromPitchRollYawDegrees( float pitch, float roll, float yaw );

	Mat44 GetMatrix() const;
	Mat44 GetRotationMatrix() const;
	float GetRotationPitch() const { return m_pitch; }
	float GetRotationRoll() const { return m_roll; }
	float GetRotationYaw() const { return m_yaw; }

	static const Mat44 GetBasisTransformMatrix( eWorldBasis basis );

public:
	Vec3 m_position                     = Vec3::ZERO;
	Vec3 m_scale                        = Vec3::ONE;
	float m_pitch = 0.f;
	float m_roll = 0.f;
	float m_yaw = 0.f;

};