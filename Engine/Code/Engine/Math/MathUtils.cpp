#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/Polygon2.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include <math.h>

constexpr float PI = 3.14159265f;

float ConvertDegreesToRadians( float degrees )
{
	return degrees * PI / 180;
}

float ConvertRadiansToDegrees( float radians )
{
	return radians * 180.f / PI;
}

float CosDegrees( float degrees )
{
	return (float)cos( ConvertDegreesToRadians( degrees ) );
}

float SinDegrees( float degrees )
{
	return (float)sin( ConvertDegreesToRadians( degrees ) );
}

float TanDegrees( float degrees )
{
	return tanf( ConvertDegreesToRadians( degrees ) );
}

float Atan2Degrees( float y, float x )
{
	return ConvertRadiansToDegrees( (float)atan2( y, x ) );
}

float GetDistance2D( const Vec2& positionA, const Vec2& positionB )
{
	const float& xDiff = positionB.x - positionA.x;
	const float& yDiff = positionB.y - positionA.y;

	return (float)sqrt( pow( xDiff, 2.f ) + pow( yDiff, 2.f ) );
}

float GetDistanceSquared2D( const Vec2& positionA, const Vec2& positionB )
{
	const float& xDiff = positionB.x - positionA.x;
	const float& yDiff = positionB.y - positionA.y;

	return (float)pow( xDiff, 2.f ) + (float)pow( yDiff, 2.f );
}

float GetDistance3D( const Vec3& positionA, const Vec3& positionB )
{
	const float& xDiff = positionB.x - positionA.x;
	const float& yDiff = positionB.y - positionA.y;
	const float& zDiff = positionB.z - positionA.z;

	return (float)sqrt( pow( xDiff, 2.f ) + pow( yDiff, 2.f ) + pow( zDiff, 2.f ) );
}

float GetDistanceSquared3D( const Vec3& positionA, const Vec3& positionB )
{
	const float& xDiff = positionB.x - positionA.x;
	const float& yDiff = positionB.y - positionA.y;
	const float& zDiff = positionB.z - positionA.z;

	return (float)pow( xDiff, 2.f ) + (float)pow( yDiff, 2.f ) + (float)pow( zDiff, 2.f );
}

float GetDistanceXY3D( const Vec3& positionA, const Vec3& positionB )
{
	const float& xDiff = positionB.x - positionA.x;
	const float& yDiff = positionB.y - positionA.y;

	return (float)sqrt( pow( xDiff, 2.f ) + pow( yDiff, 2.f ) );
}

float GetDistanceXYSquared3D( const Vec3& positionA, const Vec3& positionB )
{
	const float& xDiff = positionB.x - positionA.x;
	const float& yDiff = positionB.y - positionA.y;

	return (float)pow( xDiff, 2.f ) + (float)pow( yDiff, 2.f );
}

bool DoDiscsOverlap( const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB )
{
	const float& discsDistance = GetDistance2D( centerA, centerB );
	const float& twoRadiusAdd = radiusA + radiusB;

	return (discsDistance < twoRadiusAdd) ? true : false;
}

bool DoDiscsOverlap3D( const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB )
{
	const float& discsDistance = GetDistance3D( centerA, centerB );
	const float& twoRadiusAdd = radiusA + radiusB;

	return (discsDistance < twoRadiusAdd) ? true : false;
}

bool PushDiscOutOfPoint2D( Vec2& discCenter, float discRadius, const Vec2& solidPoint )
{
	if( IsPointInsideDisc2D( solidPoint, discCenter, discRadius ) )
	{
		Vec2 displacement = discCenter - solidPoint;
		discCenter += (discRadius - displacement.GetLength()) * displacement.GetNormalized();
	}
	return true;
}

bool DoDiscOverlapAABB2( const Vec2& discCenter, float discRadius, const AABB2& aabb2 )
{
	Vec2 nearestPointOnAABB2 = aabb2.GetNearestPoint( discCenter );
	float distance = GetDistance2D( nearestPointOnAABB2, discCenter );

	if( distance > discRadius )
		return false;

	return true;
}

bool DoAABB2Overlap( const AABB2& aabb2A, const AABB2& aabb2B )
{
	return (aabb2A.mins.x <= aabb2B.maxs.x && aabb2A.maxs.x >= aabb2B.mins.x) &&
		(aabb2A.mins.y <= aabb2B.maxs.y && aabb2A.maxs.y >= aabb2B.mins.y);
}

bool DoSpheresOverlap( const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB )
{
	const float& discsDistance = GetDistance3D( centerA, centerB );
	const float& twoRadiusAdd = radiusA + radiusB;

	return (discsDistance < twoRadiusAdd) ? true : false;
}


const Vec2 TransformPosition2D( const Vec2& position, float uniformScale, float rotationDegrees, const Vec2& translation )
{
	const Vec2& resizePosition = position * uniformScale;
	const float& radius = GetDistance2D( resizePosition, Vec2() );
	const float& theta = Atan2Degrees( resizePosition.y, resizePosition.x );

	return Vec2( radius * CosDegrees( theta + rotationDegrees ), radius * SinDegrees( theta + rotationDegrees ) ) + translation;
}

const Vec2 TransformPosition2D( const Vec2& position, const Vec2& iBasis, const Vec2& jBasis, const Vec2& translation )
{
	return Vec2( position.x * iBasis.x + position.y * jBasis.x + translation.x,
		position.x * iBasis.y + position.y * jBasis.y + translation.y );
}

const Vec3 TransformPosition3DXY( const Vec3& position, float scaleXY, float zRotationDegrees, const Vec2& translationXY )
{
	const Vec2& resizePosition = Vec2( position.x * scaleXY, position.y * scaleXY );
	const float& radius = GetDistance2D( resizePosition, Vec2() );
	const float& theta = Atan2Degrees( resizePosition.y, resizePosition.x );

	return Vec3( radius * CosDegrees( theta + zRotationDegrees ) + translationXY.x,
		radius * SinDegrees( theta + zRotationDegrees ) + translationXY.y, position.z );
}

const Vec3 TransformPosition3DXY( const Vec3& position, const Vec2& iBasisXY, const Vec2& jBasisXY, const Vec2& translationXY )
{
	return Vec3( position.x * iBasisXY.x + position.y * jBasisXY.x + translationXY.x,
		position.x * iBasisXY.y + position.y * jBasisXY.y + translationXY.y, position.z );
}

const void TransformVertex( Vertex_PCU& vertex, float uniformScale, float rotationDegrees, const Vec2& translation )
{
	const Vec2& vertexPos = Vec2( vertex.m_position.x, vertex.m_position.y );
	const Vec2& newPos = TransformPosition2D( vertexPos, uniformScale, rotationDegrees, translation );
	vertex.m_position = Vec3( newPos.x, newPos.y, vertex.m_position.z );
}

const void TransformVertexArray( const int& vertexesNum, Vertex_PCU* vertexesArray, float uniformScale, float rotationDegrees, const Vec2& translation )
{
	for( int vertexIndex = 0; vertexIndex < vertexesNum; ++vertexIndex )
	{
		TransformVertex( vertexesArray[vertexIndex], uniformScale, rotationDegrees, translation );
	}
}

const Vec2 GetNearestPointOnCapsule2D( const Vec2& refPos, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius )
{
	return Capsule2( capsuleMidStart, capsuleMidEnd, capsuleRadius ).GetNearestPoint( refPos );
}

const Vec2 GetNearestPointOnOBB2D( const Vec2& refPos, const OBB2& box )
{
	return box.GetNearestPoint( refPos );
}

const Vec2 GetNearestPointOnPolygon2D( const Vec2& refPos, const Polygon2& polygon )
{
	if( IsPointInsidePolygon2D( refPos, polygon ) )
	{
		return refPos;
	}

	return polygon.GetPointOnNearestEdge( refPos );
}

bool IsPointInsideDisc2D( const Vec2& point, const Vec2& discCenter, float discRadius )
{
	Vec2 displacement = point - discCenter;
	if( displacement.GetLength() < discRadius )
		return true;

	return false;
}

bool IsPointInsideAABB2D( const Vec2& point, const AABB2& box )
{
	return box.IsPointInside( point );
}

bool IsPointInsideCapsule2D( const Vec2& point, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius )
{
	return Capsule2( capsuleMidStart, capsuleMidEnd, capsuleRadius ).IsPointInside( point );
}

const Vec2 GetNearestPointOnAABB2D( const Vec2& referencePos, const AABB2& box )
{
	return box.GetNearestPoint( referencePos );
}

const Vec2 GetNearestPointOnDisc2D( const Vec2& point, const Vec2& discCenter, float discRadius )
{
	if( IsPointInsideDisc2D( point, discCenter, discRadius ) )
		return point;

	Vec2 displacement = point - discCenter;
	return discCenter + (displacement.GetNormalized() * discRadius);
}

const Vec2 GetNearestPointOnInfiniteLine2D( const Vec2& refPos, const Vec2& somePointOnLine, const Vec2& anotherPointOnLine )
{
	LineSegment2 line = LineSegment2( somePointOnLine, anotherPointOnLine );
	return line.GetNearestPoint( refPos );
}

const Vec2 GetNearestPointOnLineSegment2D( const Vec2& refPos, const Vec2& start, const Vec2& end )
{
	LineSegment2 line = LineSegment2( start, end );
	return line.GetNearestPoint( refPos );
}

bool PushDiscOutOfDisc2D( Vec2& pushedDiscPos, float pushedDiscRadius, const Vec2& solidDiscPos, float solidDiscRadius )
{
	if( !DoDiscsOverlap( pushedDiscPos, pushedDiscRadius, solidDiscPos, solidDiscRadius ) )
		return true;

	Vec2 displacement = pushedDiscPos - solidDiscPos;
	displacement.Normalize();
	float distance = GetDistance2D( pushedDiscPos, solidDiscPos );
	pushedDiscPos += ((pushedDiscRadius + solidDiscRadius) - distance) * displacement;
	return true;
}

bool PushDiscsOutOfEachOther2D( Vec2& aDiscPos, float aDiscRadius, Vec2& bDiscPos, float bDiscRadius )
{
	if( !DoDiscsOverlap( aDiscPos, aDiscRadius, bDiscPos, bDiscRadius ) )
		return true;

	Vec2 displacement = bDiscPos - aDiscPos;
	displacement.Normalize();
	float distance = GetDistance2D( aDiscPos, bDiscPos );
	Vec2 bCorrection = displacement * (((aDiscRadius + bDiscRadius) - distance) * 0.5f);
	bDiscPos += bCorrection;
	aDiscPos += -bCorrection;
	return true;
}

bool PushDiscOutOfAABB2D( Vec2& discCenter, float discRadius, const AABB2& solidBox )
{
	if( DoDiscOverlapAABB2( discCenter, discRadius, solidBox ) )
	{
		Vec2 nearestPoint = solidBox.GetNearestPoint( discCenter );
		Vec2 nearestDisplacement = discCenter - nearestPoint;
		if( nearestDisplacement.GetLength() < discRadius )
		{
			discCenter = nearestPoint + (nearestDisplacement.GetNormalized() * discRadius);
		}
	}
	return true;
}

float GetProjectedLength2D( const Vec2& sourceVector, const Vec2& ontoVector )
{
	float degrees = GetAngleDegreesBetweenVectors2D( sourceVector, ontoVector );
	if( degrees>90.f )
	{
		return -GetProjectedOnto2D( sourceVector, ontoVector ).GetLength();
	}
	return GetProjectedOnto2D( sourceVector, ontoVector ).GetLength();
}

const Vec2 GetProjectedOnto2D( const Vec2& sourceVector, const Vec2& ontoVector )
{
	Vec2 ontoVectorNormalized = ontoVector.GetNormalized();

	return DotProduct2D( sourceVector, ontoVectorNormalized ) * ontoVectorNormalized;
}

float GetShortestAngularDisplacement( float originDegrees, float targetDegrees )
{
	float deltaDegrees = targetDegrees - originDegrees;

	deltaDegrees = (float)fmod( deltaDegrees, 360.f );
	if( deltaDegrees >= 180.f )
	{
		return deltaDegrees - 360.f;
	}
	else if( deltaDegrees <= -180.f )
	{
		return deltaDegrees + 360.f;
	}

	return deltaDegrees;
}

float GetTurnedToward( float originDegrees, float targetDegrees, float maxTurn )
{
	float shortestAngular = GetShortestAngularDisplacement( originDegrees, targetDegrees );
	originDegrees = (float)fmod( originDegrees, 360.f );
	targetDegrees = (float)fmod( targetDegrees, 360.f );

	float answerDegrees = originDegrees;
	if( shortestAngular > 0.f )
	{
		answerDegrees = shortestAngular > maxTurn ? originDegrees+maxTurn : originDegrees+shortestAngular;
	}
	else if( shortestAngular < 0.f )
	{
		answerDegrees = abs( shortestAngular ) > maxTurn ? originDegrees-maxTurn : originDegrees+shortestAngular;
	}

	return answerDegrees;
}

float DotProduct2D( const Vec2& posA, const Vec2& posB )
{
	return (posA.x * posB.x) + (posA.y * posB.y);
}

float CrossProduct2D( const Vec2& posA, const Vec2& posB )
{
	return (posA.x * posB.y) - (posA.y * posB.x);
}

Vec3 CrossProduct3D( const Vec3& posA, const Vec3& posB )
{
	float x = posA.y * posB.z - posA.z * posB.y;
	float y = posA.z * posB.x - posA.x * posB.z;
	float z = posA.x * posB.y - posA.y * posB.x;
	return Vec3( x, y, z );
}

float SignFloat( float val )
{
	return (val >= 0.0f) ? 1.0f : -1.0f;  
}

bool IsPointInsideOBB2D( const Vec2& point, const OBB2& box )
{
	return box.IsPointInside( point );
}

bool IsPointInForwardSector2D( const Vec2& point, const Vec2& observerPos, float forwardDegrees, float apertureDegrees, float maxDist )
{
	if( GetDistance2D( point, observerPos ) > maxDist )
		return false;

	Vec2 displacement = point - observerPos;
	float displacementAngleDegrees = displacement.GetAngleDegrees();
	float shortestAngle = fabsf( GetShortestAngularDisplacement( displacementAngleDegrees, forwardDegrees ) );

	if( shortestAngle <= apertureDegrees * 0.5f )
	{
		return true;
	}

	return false;
}

bool IsPointInsidePolygon2D( const Vec2& point, const Polygon2& polygon )
{
	if( !polygon.IsValid() )
	{
		return false;
	}
	if( polygon.IsConvex() )
	{
		std::vector<Vec2> points = polygon.GetPoints();

		for( int index = 0; index < (int)points.size(); index++ )
		{
			Vec2 vecA = Vec2::ZERO;
			Vec2 vecB = Vec2::ZERO;
			if( index == polygon.GetEdgeCount() - 1 )
			{
				vecA = points[index] - point;
				vecB = points[0] - point;
			}
			else
			{
				vecA = points[index] - point;
				vecB = points[index+1] - point;
			}

			if( CrossProduct2D( vecA, vecB ) < 0 )
			{
				return false;
			}
		}
		return true;
	}

	return false;
}

float GetAngleDegreesBetweenVectors2D( const Vec2& aVec2, const Vec2& bVec2 )
{
	return fabsf( GetShortestAngularDisplacement( aVec2.GetAngleDegrees(), bVec2.GetAngleDegrees() ) );
}

int GetTaxicabDistance2D( const IntVec2& aVec2, const IntVec2& bVec2 )
{
	IntVec2 diff = aVec2 - bVec2;
	return abs( diff.x ) + abs( diff.y );
}

float Interpolate( float a, float b, float fractionOfB )
{
	return a + fractionOfB * (b - a);
}

float RangeMap( float inputRangeBegin, float inputRangeEnd, float outputRangeBegin, float outputRangeEnd, float inputValue )
{
	float inputDisplacement = inputValue - inputRangeBegin;
	float inputRange = inputRangeEnd - inputRangeBegin;
	float fraction = inputDisplacement / inputRange;
	float outRange = outputRangeEnd - outputRangeBegin;
	float outDisplacement = fraction * outRange;

	return outputRangeBegin + outDisplacement;
}

Vec3 RangeMap( Vec3 inputRangeBegin, Vec3 inputRangeEnd, Vec3 outputRangeBegin, Vec3 outputRangeEnd, Vec3 inputValue )
{
	Vec3 inputDisplacement = inputValue - inputRangeBegin;
	Vec3 inputRange = inputRangeEnd - inputRangeBegin;
	Vec3 fraction = inputDisplacement / inputRange;
	Vec3 outRange = outputRangeEnd - outputRangeBegin;
	Vec3 outDisplacement = fraction * outRange;

	return outputRangeBegin + outDisplacement;
}

Vec2 RangeMap( float inputRangeBegin, float inputRangeEnd, Vec2 outputRangeBegin, Vec2 outputRangeEnd, float inputValue )
{
	float inputDisplacement = inputValue - inputRangeBegin;
	float inputRange = inputRangeEnd - inputRangeBegin;
	float fraction = inputDisplacement == 0.f || inputRange == 0.f ? 0.f : inputDisplacement / inputRange;
	Vec2 outRange = outputRangeEnd - outputRangeBegin;
	Vec2 outDisplacement = fraction * outRange;

	return outputRangeBegin + outDisplacement;
}

float Clamp( float value, float min, float max )
{
	if( value < min )
	{
		return min;
	}
	else if( value > max )
	{
		return max;
	}
	return value;
}

int Clamp( int value, int min, int max )
{
	if( value < min )
	{
		return min;
	}
	else if( value > max )
	{
		return max;
	}
	return value;
}

float ClampZeroToOne( float value )
{
	return Clamp( value, 0.f, 1.f );
}

float Round( float value )
{
	return roundf( value );
}

float Floor( float value )
{
	return floorf( value );
}

int RoundDownToInt( float value )
{
	float roundFloat = floorf( value );
	return static_cast<int>(roundFloat);
}

int RoundToNearestInt( float value )
{
	float roundFloat = roundf( value );
	return static_cast<int>(roundFloat);
}

float GetMax( float numA, float numB )
{
	return fmaxf( numA, numB );
}

float GetMin( float numA, float numB )
{
	return fminf( numA, numB );
}
