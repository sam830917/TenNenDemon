#pragma once
// Forward type declarations
struct Vec2;
struct IntVec2;
struct Vec3;
struct AABB2;
struct Vertex_PCU;
struct OBB2;
struct FloatRange;
class  Polygon2;

float		GetShortestAngularDisplacement( float originDegrees, float targetDegrees );
float		GetTurnedToward( float originDegrees, float targetDegrees, float maxTurn );
float		DotProduct2D( const Vec2& posA, const Vec2& posB );
float		CrossProduct2D( const Vec2& posA, const Vec2& posB );
Vec3		CrossProduct3D( const Vec3& posA, const Vec3& posB );
float		SignFloat( float val );
int			GetTaxicabDistance2D( const IntVec2& aVec2, const IntVec2& bVec2 );

// Lerp and Clamp
float		Interpolate( float a, float b, float fractionOfB );
float		RangeMap( float inputRangeBegin, float inputRangeEnd, float outputRangeBegin, float outputRangeEnd, float inputValue );
Vec3		RangeMap( Vec3 inputRangeBegin, Vec3 inputRangeEnd, Vec3 outputRangeBegin, Vec3 outputRangeEnd, Vec3 inputValue );
Vec2		RangeMap( float inputRangeBegin, float inputRangeEnd, Vec2 outputRangeBegin, Vec2 outputRangeEnd, float inputValue );
float		Clamp( float value, float min, float max );
int			Clamp( int value, int min, int max );
float		ClampZeroToOne( float value );
float		Round( float value );
float		Floor( float value );
int			RoundDownToInt( float value );
int			RoundToNearestInt( float value );
float		GetMax( float numA, float numB );
float		GetMin( float numA, float numB );

// Angle utilities
float		ConvertDegreesToRadians( float degrees );
float		ConvertRadiansToDegrees( float radians );
float		CosDegrees( float degrees );
float		SinDegrees( float degrees );
float		TanDegrees( float degrees );
float		Atan2Degrees( float y, float x );

// Basic 2D & 3D utilities
float		GetDistance2D( const Vec2& positionA, const Vec2& positionB );
float		GetDistanceSquared2D( const Vec2& positionA, const Vec2& positionB );
float		GetDistance3D( const Vec3& positionA, const Vec3& positionB );
float		GetDistanceSquared3D( const Vec3& positionA, const Vec3& positionB );
float		GetDistanceXY3D( const Vec3& positionA, const Vec3& positionB );
float		GetDistanceXYSquared3D( const Vec3& positionA, const Vec3& positionB );

// Geometric query utilities
FloatRange	GetRangeOnProjectedAxis( int numPoints, const Vec2* points, const Vec2& relativeToPos, const Vec2& axisNormal );
bool		DoOBBAndOBBOverlap2D( const OBB2& boxA, const OBB2& boxB );
bool		DoDiscsOverlap( const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB );
bool		DoDiscsOverlap3D( const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB );
bool		DoDiscOverlapAABB2( const Vec2& discCenter, float discRadius, const AABB2& aabb2 );
bool		DoAABB2Overlap( const AABB2& aabb2A, const AABB2& aabb2B );
bool		DoSpheresOverlap( const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB );

bool		PushDiscOutOfPoint2D( Vec2& discCenter, float discRadius, const Vec2& solidPoint );
bool		PushDiscOutOfDisc2D( Vec2& pushedDiscPos, float pushedDiscRadius, const Vec2& solidDiscPos, float solidDiscRadius );
bool		PushDiscsOutOfEachOther2D( Vec2& aDiscPos, float aDiscRadius, Vec2& bDiscPos, float bDiscRadius );
bool		PushDiscOutOfAABB2D( Vec2& discCenter, float discRadius, const AABB2& solidBox );

float		GetProjectedLength2D( const Vec2& sourceVector, const Vec2& ontoVector );
const Vec2	GetProjectedOnto2D( const Vec2& sourceVector, const Vec2& ontoVector );
float		GetAngleDegreesBetweenVectors2D( const Vec2& aVec2, const Vec2& bVec2 );

const Vec2  GetNearestPointOnAABB2D( const Vec2& referencePos, const AABB2& box );
const Vec2	GetNearestPointOnDisc2D( const Vec2& point, const Vec2& discCenter, float discRadius );
const Vec2  GetNearestPointOnInfiniteLine2D( const Vec2& refPos, const Vec2& somePointOnLine, const Vec2& anotherPointOnLine );
const Vec2  GetNearestPointOnLineSegment2D( const Vec2& refPos, const Vec2& start, const Vec2& end );
const Vec2  GetNearestPointOnCapsule2D( const Vec2& refPos, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius );
const Vec2  GetNearestPointOnOBB2D( const Vec2& refPos, const OBB2& box );
const Vec2  GetNearestPointOnPolygon2D( const Vec2& refPos, const Polygon2& polygon );

bool		IsPointInsideDisc2D( const Vec2& point, const Vec2& discCenter, float discRadius );
bool		IsPointInsideAABB2D( const Vec2& point, const AABB2& box );
bool		IsPointInsideCapsule2D( const Vec2& point, const Vec2& capsuleMidStart, const Vec2& capsuleMidEnd, float capsuleRadius );
bool		IsPointInsideOBB2D( const Vec2& point, const OBB2& box );
bool		IsPointInsidePolygon2D( const Vec2& point, const Polygon2& polygon );
bool		IsPointInForwardSector2D( const Vec2& point, const Vec2& observerPos, float forwardDegrees, float apertureDegrees, float maxDist );

// Transform utilities
const Vec2	TransformPosition2D( const Vec2& position, float uniformScale, float rotationDegrees, const Vec2& translation );
const Vec2	TransformPosition2D( const Vec2& position, const Vec2& iBasis, const Vec2& jBasis, const Vec2& translation );
const Vec3	TransformPosition3DXY( const Vec3& position, float scaleXY, float zRotationDegrees, const Vec2& translationXY );
const Vec3	TransformPosition3DXY( const Vec3& position, const Vec2& iBasisXY, const Vec2& jBasisXY, const Vec2& translationXY );
const void	TransformVertex( Vertex_PCU& vertex, float uniformScale, float rotationDegrees, const Vec2& translation );
const void	TransformVertexArray( const int& vertexesNum, Vertex_PCU* vertexesArray, float uniformScale, float rotationDegrees, const Vec2& translation );

//	SmoothStart, SmoothStop
float		SmoothStart2( float t );		// [0,1] quadratic ease-in
float		SmoothStart3( float t );		// [0,1] cubic ease-in
float		SmoothStart4( float t );		// [0,1] quartic ease-in
float		SmoothStart5( float t );		// [0,1] quintic ease-in
float		SmoothStop2( float t );			// [0,1] quadratic ease-in
float		SmoothStop3( float t );			// [0,1] cubic ease-in
float		SmoothStop4( float t );			// [0,1] quartic ease-in
float		SmoothStop5( float t );			// [0,1] quintic ease-in
float		SmoothStep3( float t );			// [0,1] cubic ease-in