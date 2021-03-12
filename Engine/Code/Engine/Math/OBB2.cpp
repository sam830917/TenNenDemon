#include "OBB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

OBB2::OBB2( const OBB2& copyFrom )
{
	m_center = copyFrom.m_center;
	m_halfDimensions  = copyFrom.m_halfDimensions;
	m_iBasisNormal = copyFrom.m_iBasisNormal;
}

OBB2::OBB2( const Vec2& center, const Vec2& fullDimensions, const Vec2& iBasisNormal )
{
	m_center = center;
	m_halfDimensions  = fullDimensions * 0.5f;
	m_iBasisNormal = iBasisNormal;
}

OBB2::OBB2( const Vec2& center, const Vec2& fullDimensions, float orientationDegrees )
{
	m_center = center;
	m_halfDimensions  = fullDimensions * 0.5f;
	Vec2 iBasis = Vec2( 1.f, 0.f );
	iBasis.SetAngleDegrees( orientationDegrees );
	m_iBasisNormal = iBasis;
}

OBB2::OBB2( const AABB2& asAxisAlignedBox, float orientationDegrees )
{
	m_center = asAxisAlignedBox.GetCenter();
	m_halfDimensions = asAxisAlignedBox.GetDimensions() * 0.5f;
	Vec2 iBasis = Vec2( 1.f, 0.f );
	iBasis.SetAngleDegrees( orientationDegrees );
	m_iBasisNormal = iBasis;
}

bool OBB2::IsPointInside( const Vec2& point ) const
{
	Vec2 displacement = point - m_center;
	Vec2 pointBasisij = Vec2( DotProduct2D( displacement, GetIBasisNormal() ), DotProduct2D( displacement, GetJBasisNormal() ) );
	if ( fabsf( pointBasisij.x ) < m_halfDimensions.x && fabsf( pointBasisij.y ) < m_halfDimensions.y )
	{
		return true;
	}
	return false;
}

const Vec2 OBB2::GetCenter() const
{
	return m_center;
}

const Vec2 OBB2::GetDimensions() const
{
	return m_halfDimensions * 2.f;
}

const Vec2 OBB2::GetIBasisNormal() const
{
	return m_iBasisNormal;
}

const Vec2 OBB2::GetJBasisNormal() const
{
	return Vec2( -m_iBasisNormal.y, m_iBasisNormal.x );
}

float OBB2::GetOrientationDegrees() const
{
	return m_iBasisNormal.GetAngleDegrees();
}

const Vec2 OBB2::GetNearestPoint( const Vec2& referencePos ) const
{
	Vec2 displacement = referencePos - m_center;
	Vec2 pointBasisij = Vec2( DotProduct2D( displacement, GetIBasisNormal() ), DotProduct2D( displacement, GetJBasisNormal() ) );
	Vec2 nBasisij = Vec2( Clamp( pointBasisij.x, -m_halfDimensions.x, m_halfDimensions.x ), Clamp( pointBasisij.y, -m_halfDimensions.y, m_halfDimensions.y ) );

	return m_center + nBasisij.x * GetIBasisNormal() + nBasisij.y * GetJBasisNormal();
}

const Vec2 OBB2::GetPointAtUV( const Vec2& uvCoordsZeroToOne ) const
{
	// TODO
	UNUSED( uvCoordsZeroToOne );
	return Vec2();
}

const Vec2 OBB2::GetUVForPoint( const Vec2& point ) const
{
	// TODO
	UNUSED( point );
	return Vec2();
}

void OBB2::GetCornerPosition( Vec2* out_fourPoints ) const
{
	Vec2 iBasis = GetIBasisNormal() * m_halfDimensions.x;
	Vec2 jBasis = GetJBasisNormal() * m_halfDimensions.y;

	out_fourPoints[0] = m_center - iBasis - jBasis;		// Bottom Left
	out_fourPoints[1] = m_center + iBasis - jBasis;		// Bottom Right
	out_fourPoints[2] = m_center - iBasis + jBasis;		// Top Left
	out_fourPoints[3] = m_center + iBasis + jBasis;		// Top Right
}

void OBB2::Translate( const Vec2& translation )
{
	m_center += translation;
}

void OBB2::SetCenter( const Vec2& newCenter )
{
	m_center = newCenter;
}

void OBB2::SetDimensions( const Vec2& newDimensions )
{
	m_halfDimensions = newDimensions * 0.5f;
}

void OBB2::SetOrientationDegrees( float completelyNewAbsoluteOrientation )
{
	Vec2 iBasis = Vec2( 1.f, 0.f );
	iBasis.SetAngleDegrees( completelyNewAbsoluteOrientation );
	m_iBasisNormal = iBasis;
}

void OBB2::RotateByDegrees( float relativeRotationDegrees )
{
	m_iBasisNormal.RotateDegrees( relativeRotationDegrees );
}

void OBB2::StretchToIncludePoint( const Vec2& point )
{
	// TODO
	UNUSED( point );
}

void OBB2::Fix()
{
	// TODO
}

void OBB2::operator=( const OBB2& assignFrom )
{
	m_center = assignFrom.m_center;
	m_halfDimensions  = assignFrom.m_halfDimensions;
	m_iBasisNormal = assignFrom.m_iBasisNormal;
}
