#pragma once
#include "Vec2.hpp"
#include "Vec3.hpp"

struct OBB3
{
	Vec3	m_center;
	Vec3	m_halfDimensions;
	Vec3	m_iBasisNormal = Vec3( 1.f, 0.f, 0.f );
	Vec3	m_jBasisNormal = Vec3( 0.f, 1.f, 0.f );
	Vec3	m_kBasisNormal = Vec3( 0.f, 0.f, 1.f );

	OBB3() = default;
	~OBB3() = default;
	explicit OBB3( const Vec3& center, const Vec3& fullDimensions );

	Vec3 GetIBasisNormal() const { return m_iBasisNormal; }
	Vec3 GetJBasisNormal() const { return m_jBasisNormal; }
	Vec3 GetKBasisNormal() const { return m_kBasisNormal; }

	void GetCornerPosition( Vec3* out_fourPoints ) const;
};