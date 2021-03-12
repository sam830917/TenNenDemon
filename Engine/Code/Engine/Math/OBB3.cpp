#include "Engine/Math/OBB3.hpp"

OBB3::OBB3( const Vec3& center, const Vec3& fullDimensions )
	:m_center(center),
	m_halfDimensions(fullDimensions * 0.5f)
{
}

void OBB3::GetCornerPosition( Vec3* out_fourPoints ) const
{
	Vec3 iBasis = GetIBasisNormal() * m_halfDimensions.x;
	Vec3 jBasis = GetJBasisNormal() * m_halfDimensions.y;
	Vec3 kBasis = GetKBasisNormal() * m_halfDimensions.z;

	out_fourPoints[0] = m_center - iBasis - jBasis + (kBasis * 0.5f);		// Bottom Left
	out_fourPoints[1] = m_center + iBasis - jBasis + (kBasis * 0.5f);		// Bottom Right
	out_fourPoints[2] = m_center - iBasis + jBasis + (kBasis * 0.5f);		// Top Left
	out_fourPoints[3] = m_center + iBasis + jBasis + (kBasis * 0.5f);		// Top Right

	out_fourPoints[4] = m_center - iBasis - jBasis - (kBasis * 0.5f);		// Bottom Left
	out_fourPoints[5] = m_center + iBasis - jBasis - (kBasis * 0.5f);		// Bottom Right
	out_fourPoints[6] = m_center - iBasis + jBasis - (kBasis * 0.5f);		// Top Left
	out_fourPoints[7] = m_center + iBasis + jBasis - (kBasis * 0.5f);		// Top Right
}
