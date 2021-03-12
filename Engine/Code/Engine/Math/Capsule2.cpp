#include "Capsule2.hpp"
#include "Engine/Math/MathUtils.hpp"

Capsule2::Capsule2( const Vec2& boneStart, const Vec2& boneEnd, float radius )
	:m_boneStart( boneStart ),
	m_boneEnd( boneEnd ), 
	m_radius( radius )
{
}

bool Capsule2::IsPointInside( const Vec2& point ) const
{
	Vec2 capsuleDisplacement = m_boneEnd - m_boneStart;
	Vec2 pointAndStartDisplacement = point - m_boneStart;
	Vec2 pointAndEndDisplacement = point - m_boneEnd;
	float pointAndStartdot = DotProduct2D( capsuleDisplacement, pointAndStartDisplacement );
	float pointAndEnddot = DotProduct2D( capsuleDisplacement, pointAndEndDisplacement );
	// Point on start position side
	if ( pointAndStartdot < 0.f )
	{
		if ( pointAndStartDisplacement.GetLength() < m_radius )
		{
			return true;
		}
	}
	// Point on end position side
	else if ( pointAndEnddot > 0.f )
	{
		if( pointAndEndDisplacement.GetLength() < m_radius )
		{
			return true;
		}
	}
	// Point on mid side
	else
	{
		Vec2 iBasis = capsuleDisplacement.GetNormalized();
		Vec2 jBasis = capsuleDisplacement.GetNormalized().GetRotated90Degrees();

		Vec2 displacement = point - m_boneStart;
		Vec2 pointBasisij = Vec2( DotProduct2D( displacement, iBasis ), DotProduct2D( displacement, jBasis ) );
		if( fabsf( pointBasisij.x ) < capsuleDisplacement.GetLength() && fabsf( pointBasisij.y ) < m_radius )
		{
			return true;
		}
	}

	return false;
}

const Vec2 Capsule2::GetNearestPoint( const Vec2& referencePoint ) const
{
	Vec2 capsuleDisplacement = m_boneEnd - m_boneStart;
	Vec2 pointAndStartDisplacement = referencePoint - m_boneStart;
	Vec2 pointAndEndDisplacement = referencePoint - m_boneEnd;
	float pointAndStartdot = DotProduct2D( capsuleDisplacement, pointAndStartDisplacement );
	float pointAndEnddot = DotProduct2D( capsuleDisplacement, pointAndEndDisplacement );
	// Point on start position side
	if( pointAndStartdot < 0.f )
	{
		if( pointAndStartDisplacement.GetLength() < m_radius )
		{
			return referencePoint;
		}
		return m_boneStart + pointAndStartDisplacement.GetNormalized() * m_radius;
	}
	// Point on end position side
	else if( pointAndEnddot > 0.f )
	{
		if( pointAndEndDisplacement.GetLength() < m_radius )
		{
			return referencePoint;
		}
		return m_boneEnd + pointAndEndDisplacement.GetNormalized() * m_radius;
	}
	// Point on mid side
	else
	{
		Vec2 iBasis = capsuleDisplacement.GetNormalized();
		Vec2 jBasis = capsuleDisplacement.GetNormalized().GetRotated90Degrees();

		Vec2 displacement = referencePoint - m_boneStart;
		Vec2 pointBasisij = Vec2( DotProduct2D( displacement, iBasis ), DotProduct2D( displacement, jBasis ) );
		if( fabsf( pointBasisij.x ) < capsuleDisplacement.GetLength() && fabsf( pointBasisij.y ) < m_radius )
		{
			return referencePoint;
		}

		Vec2 bonePoint = GetProjectedOnto2D( pointAndStartDisplacement, capsuleDisplacement ) + m_boneStart;
		Vec2 bonePointDisplacement = referencePoint - bonePoint;
		return bonePoint + bonePointDisplacement.GetNormalized() * m_radius;
	}
}

void Capsule2::Translate( const Vec2& translation )
{
	m_boneStart += translation;
	m_boneEnd += translation;
}

void Capsule2::RotateDegrees( float relativeRotationDegrees )
{
	Vec2 capsuleDisplacement = m_boneEnd - m_boneStart;
	Vec2 center = m_boneStart + capsuleDisplacement * 0.5;
	Vec2 centerToEndDis = m_boneEnd - center;
	Vec2 centerToStartDis = m_boneStart - center;
	centerToStartDis.RotateDegrees( relativeRotationDegrees );
	centerToEndDis.RotateDegrees( relativeRotationDegrees );

	m_boneStart = center + centerToStartDis;
	m_boneEnd = center + centerToEndDis;
}
