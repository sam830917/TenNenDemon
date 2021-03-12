#include "Mat44.hpp"
#include "MathUtils.hpp"
#include "Engine/Renderer/Transform.hpp"

const Mat44 Mat44::IDENTITY = Mat44();

Mat44::Mat44( float* sixteenValuesBasisMajor )
{
	Ix = sixteenValuesBasisMajor[0];
	Iy = sixteenValuesBasisMajor[1];
	Iz = sixteenValuesBasisMajor[2];
	Iw = sixteenValuesBasisMajor[3];

	Jx = sixteenValuesBasisMajor[4];
	Jy = sixteenValuesBasisMajor[5];
	Jz = sixteenValuesBasisMajor[6];
	Jw = sixteenValuesBasisMajor[7];

	Kx = sixteenValuesBasisMajor[8];
	Ky = sixteenValuesBasisMajor[9];
	Kz = sixteenValuesBasisMajor[10];
	Kw = sixteenValuesBasisMajor[11];

	Tx = sixteenValuesBasisMajor[12];
	Ty = sixteenValuesBasisMajor[13];
	Tz = sixteenValuesBasisMajor[14];
	Tw = sixteenValuesBasisMajor[15];
}

Mat44::Mat44( const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D )
{
	Ix = iBasis2D.x;
	Iy = iBasis2D.y;

	Jx = jBasis2D.x;
	Jy = jBasis2D.y;

	Tx = translation2D.x;
	Ty = translation2D.y;
}

Mat44::Mat44( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D )
{
	Ix = iBasis3D.x;
	Iy = iBasis3D.y;
	Iz = iBasis3D.z;

	Jx = jBasis3D.x;
	Jy = jBasis3D.y;
	Jz = jBasis3D.z;

	Kx = kBasis3D.x;
	Ky = kBasis3D.y;
	Kz = kBasis3D.z;

	Tx = translation3D.x;
	Ty = translation3D.y;
	Tz = translation3D.z;
}

Mat44::Mat44( const Vec4& iBasisHomogeneous, const Vec4& jBasisHomogeneous, const Vec4& kBasisHomogeneous, const Vec4& translationHomogeneous )
{
	Ix = iBasisHomogeneous.x;
	Iy = iBasisHomogeneous.y;
	Iz = iBasisHomogeneous.z;
	Iw = iBasisHomogeneous.w;

	Jx = jBasisHomogeneous.x;
	Jy = jBasisHomogeneous.y;
	Jz = jBasisHomogeneous.z;
	Jw = jBasisHomogeneous.w;

	Kx = kBasisHomogeneous.x;
	Ky = kBasisHomogeneous.y;
	Kz = kBasisHomogeneous.z;
	Kw = kBasisHomogeneous.w;

	Tx = translationHomogeneous.x;
	Ty = translationHomogeneous.y;
	Tz = translationHomogeneous.z;
	Tw = translationHomogeneous.w;
}

const Vec2 Mat44::TransformVector2D( const Vec2& vectorQuantity ) const
{
	return Vec2( Ix * vectorQuantity.x + Jx * vectorQuantity.y, Iy * vectorQuantity.x + Jy * vectorQuantity.y );
}

const Vec3 Mat44::TransformVector3D( const Vec3& vectorQuantity ) const
{
	return Vec3( 
		Ix * vectorQuantity.x + Jx * vectorQuantity.y + Kx * vectorQuantity.z, 
		Iy * vectorQuantity.x + Jy * vectorQuantity.y + Ky * vectorQuantity.z,
		Iz * vectorQuantity.x + Jz * vectorQuantity.y + Kz * vectorQuantity.z );
}

const Vec2 Mat44::TransformPosition2D( const Vec2& position ) const
{
	return Vec2( Ix * position.x + Jx * position.y + Tx, Iy * position.x + Jy * position.y + Ty );
}

const Vec3 Mat44::TransformPosition3D( const Vec3& position ) const
{
	return Vec3( 
		Ix * position.x + Jx * position.y + Kx * position.z + Tx,
		Iy * position.x + Jy * position.y + Ky * position.z + Ty,
		Iz * position.x + Jz * position.y + Kz * position.z + Tz );
}

const Vec4 Mat44::TransformHomogeneousPoint3D( const Vec4& homogeneousPoint ) const
{
	return Vec4( 
		Ix * homogeneousPoint.x + Jx * homogeneousPoint.y + Kx * homogeneousPoint.z + Tx * homogeneousPoint.w,
		Iy * homogeneousPoint.x + Jy * homogeneousPoint.y + Ky * homogeneousPoint.z + Ty * homogeneousPoint.w,
		Iz * homogeneousPoint.x + Jz * homogeneousPoint.y + Kz * homogeneousPoint.z + Tz * homogeneousPoint.w,
		Iw * homogeneousPoint.x + Jw * homogeneousPoint.y + Kw * homogeneousPoint.z + Tw * homogeneousPoint.w );
}

const Vec2 Mat44::GetIBasis2D() const
{
	return Vec2( Ix, Iy );
}

const Vec2 Mat44::GetJBasis2D() const
{
	return Vec2( Jx, Jy );
}

const Vec2 Mat44::GetTranslation2D() const
{
	return Vec2( Tx, Ty );
}

const Vec3 Mat44::GetIBasis3D() const
{
	return Vec3( Ix, Iy, Iz );
}

const Vec3 Mat44::GetJBasis3D() const
{
	return Vec3( Jx, Jy, Jz );
}

const Vec3 Mat44::GetKBasis3D() const
{
	return Vec3( Kx, Ky, Kz );
}

const Vec3 Mat44::GetTranslation3D() const
{
	return Vec3( Tx, Ty, Tz );
}

const Vec4 Mat44::GetIBasis4D() const
{
	return Vec4( Ix, Iy, Iz, Iw );
}

const Vec4 Mat44::GetJBasis4D() const
{
	return Vec4( Jx, Jy, Jz, Jw );
}

const Vec4 Mat44::GetKBasis4D() const
{
	return Vec4( Kx, Ky, Kz, Kw );
}

const Vec4 Mat44::GetTranslation4D() const
{
	return Vec4( Tx, Ty, Tz, Tw );
}

const Mat44 Mat44::GetInvert() const
{
	double inv[16];
	double det;
	double m[16];
	const float* mat = GetAsFloatArray();
	for( int i = 0; i < 16; ++i ) 
	{
		m[i] = (double)mat[i];
	}

	inv[0] = m[5]  * m[10] * m[15] -
		m[5]  * m[11] * m[14] -
		m[9]  * m[6]  * m[15] +
		m[9]  * m[7]  * m[14] +
		m[13] * m[6]  * m[11] -
		m[13] * m[7]  * m[10];

	inv[4] = -m[4]  * m[10] * m[15] +
		m[4]  * m[11] * m[14] +
		m[8]  * m[6]  * m[15] -
		m[8]  * m[7]  * m[14] -
		m[12] * m[6]  * m[11] +
		m[12] * m[7]  * m[10];

	inv[8] = m[4]  * m[9]  * m[15] -
		m[4]  * m[11] * m[13] -
		m[8]  * m[5]  * m[15] +
		m[8]  * m[7]  * m[13] +
		m[12] * m[5]  * m[11] -
		m[12] * m[7]  * m[9];

	inv[12] = -m[4]  * m[9]  * m[14] +
		m[4]  * m[10] * m[13] +
		m[8]  * m[5]  * m[14] -
		m[8]  * m[6]  * m[13] -
		m[12] * m[5]  * m[10]  +
		m[12] * m[6]  * m[9];

	inv[1] = -m[1]  * m[10] * m[15] +
		m[1]  * m[11] * m[14] +
		m[9]  * m[2]  * m[15] -
		m[9]  * m[3]  * m[14] -
		m[13] * m[2]  * m[11] +
		m[13] * m[3]  * m[10];

	inv[5] = m[0]  * m[10] * m[15] -
		m[0]  * m[11] * m[14] -
		m[8]  * m[2]  * m[15] +
		m[8]  * m[3]  * m[14] +
		m[12] * m[2]  * m[11] -
		m[12] * m[3]  * m[10];

	inv[9] = -m[0]  * m[9]  * m[15] +
		m[0]  * m[11] * m[13] +
		m[8]  * m[1]  * m[15] -
		m[8]  * m[3]  * m[13] -
		m[12] * m[1]  * m[11] +
		m[12] * m[3]  * m[9];

	inv[13] = m[0]  * m[9]  * m[14] -
		m[0]  * m[10] * m[13] -
		m[8]  * m[1]  * m[14] +
		m[8]  * m[2]  * m[13] +
		m[12] * m[1]  * m[10] -
		m[12] * m[2]  * m[9];

	inv[2] = m[1]  * m[6] * m[15] -
		m[1]  * m[7] * m[14] -
		m[5]  * m[2] * m[15] +
		m[5]  * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0]  * m[6] * m[15] +
		m[0]  * m[7] * m[14] +
		m[4]  * m[2] * m[15] -
		m[4]  * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0]  * m[5] * m[15] -
		m[0]  * m[7] * m[13] -
		m[4]  * m[1] * m[15] +
		m[4]  * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0]  * m[5] * m[14] +
		m[0]  * m[6] * m[13] +
		m[4]  * m[1] * m[14] -
		m[4]  * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
	det = 1.0 / det;

	float retData[16];
	for( int i = 0; i < 16; i++ ) 
	{
		retData[i] = (float)(inv[i] * det);
	}
	Mat44 ret = Mat44( retData );

	return ret;
}

void Mat44::SetTranslation2D( const Vec2& translation2D )
{
	Tx = translation2D.x;
	Ty = translation2D.y;
}

void Mat44::SetTranslation3D( const Vec3& translation3D )
{
	Tx = translation3D.x;
	Ty = translation3D.y;
	Tz = translation3D.z;
}

void Mat44::SetBasisVectors2D( const Vec2& iBasis2D, const Vec2& jBasis2D )
{
	Ix = iBasis2D.x;
	Iy = iBasis2D.y;

	Jx = jBasis2D.x;
	Jy = jBasis2D.y;
}

void Mat44::SetBasisVectors2D( const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D )
{
	Ix = iBasis2D.x;
	Iy = iBasis2D.y;

	Jx = jBasis2D.x;
	Jy = jBasis2D.y;

	Tx = translation2D.x;
	Ty = translation2D.y;
}

void Mat44::SetBasisVectors3D( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D )
{
	Ix = iBasis3D.x;
	Iy = iBasis3D.y;
	Iz = iBasis3D.z;

	Jx = jBasis3D.x;
	Jy = jBasis3D.y;
	Jz = jBasis3D.z;

	Kx = kBasis3D.x;
	Ky = kBasis3D.y;
	Kz = kBasis3D.z;
}

void Mat44::SetBasisVectors3D( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D )
{
	Ix = iBasis3D.x;
	Iy = iBasis3D.y;
	Iz = iBasis3D.z;

	Jx = jBasis3D.x;
	Jy = jBasis3D.y;
	Jz = jBasis3D.z;

	Kx = kBasis3D.x;
	Ky = kBasis3D.y;
	Kz = kBasis3D.z;

	Tx = translation3D.x;
	Ty = translation3D.y;
	Tz = translation3D.z;
}

void Mat44::SetBasisVectors4D( const Vec4& iBasis4D, const Vec4& jBasis4D, const Vec4& kBasis4D, const Vec4& translation4D )
{
	Ix = iBasis4D.x;
	Iy = iBasis4D.y;
	Iz = iBasis4D.z;
	Iw = iBasis4D.w;

	Jx = jBasis4D.x;
	Jy = jBasis4D.y;
	Jz = jBasis4D.z;
	Jw = jBasis4D.w;

	Kx = kBasis4D.x;
	Ky = kBasis4D.y;
	Kz = kBasis4D.z;
	Kw = kBasis4D.w;

	Tx = translation4D.x;
	Ty = translation4D.y;
	Tz = translation4D.z;
	Tw = translation4D.w;
}

void Mat44::RotateXDegrees( float degreesAboutX )
{
	Mat44 rotationX = rotationX.CreateXRotationDegrees( degreesAboutX );

	TransformBy( rotationX );
}

void Mat44::RotateYDegrees( float degreesAboutY )
{
	Mat44 rotationY = rotationY.CreateYRotationDegrees( degreesAboutY );

	TransformBy( rotationY );
}

void Mat44::RotateZDegrees( float degreesAboutZ )
{
	Mat44 rotationZ = rotationZ.CreateZRotationDegrees( degreesAboutZ );

	TransformBy( rotationZ );
}

void Mat44::Translate2D( const Vec2& translationXY )
{
	Mat44 translateMat = Mat44::IDENTITY;
	translateMat.SetTranslation2D( translationXY );

	TransformBy( translateMat );
}

void Mat44::Translate3D( const Vec3& translation3D )
{
	Mat44 translateMat = Mat44::IDENTITY;
	translateMat.SetTranslation3D( translation3D );

	TransformBy( translateMat );
}

void Mat44::ScaleUniform2D( float uniformScaleXY )
{
	Mat44 translateMat = Mat44::IDENTITY;
	translateMat.Ix *= uniformScaleXY;
	translateMat.Iy *= uniformScaleXY;
	translateMat.Jx *= uniformScaleXY;
	translateMat.Jy *= uniformScaleXY;

	TransformBy( translateMat );
}

void Mat44::ScaleNonUniform2D( const Vec2& scaleFactorsXY )
{
	Mat44 translateMat = Mat44::IDENTITY;
	translateMat.Ix *= scaleFactorsXY.x;
	translateMat.Iy *= scaleFactorsXY.y;
	translateMat.Jx *= scaleFactorsXY.x;
	translateMat.Jy *= scaleFactorsXY.y;

	TransformBy( translateMat );
}

void Mat44::ScaleUniform3D( float uniformScaleXYZ )
{
	Mat44 translateMat = Mat44::IDENTITY;
	translateMat.Ix *= uniformScaleXYZ;
	translateMat.Iy *= uniformScaleXYZ;
	translateMat.Iz *= uniformScaleXYZ;

	translateMat.Jx *= uniformScaleXYZ;
	translateMat.Jy *= uniformScaleXYZ;
	translateMat.Jz *= uniformScaleXYZ;

	translateMat.Kx *= uniformScaleXYZ;
	translateMat.Ky *= uniformScaleXYZ;
	translateMat.Kz *= uniformScaleXYZ;

	TransformBy( translateMat );
}

void Mat44::ScaleNonUniform3D( const Vec3& scaleFactorsXYZ )
{
	Mat44 translateMat = Mat44::IDENTITY;
	translateMat.Ix *= scaleFactorsXYZ.x;
	translateMat.Iy *= scaleFactorsXYZ.y;
	translateMat.Iz *= scaleFactorsXYZ.z;

	translateMat.Jx *= scaleFactorsXYZ.x;
	translateMat.Jy *= scaleFactorsXYZ.y;
	translateMat.Jz *= scaleFactorsXYZ.z;

	translateMat.Kx *= scaleFactorsXYZ.x;
	translateMat.Ky *= scaleFactorsXYZ.y;
	translateMat.Kz *= scaleFactorsXYZ.z;

	TransformBy( translateMat );
}

void Mat44::TransformBy( const Mat44& arbitraryTransformationToAppend )
{
	const Mat44& translateMat = arbitraryTransformationToAppend;
	Mat44 thisMat = Mat44( GetIBasis4D(), GetJBasis4D(), GetKBasis4D(), GetTranslation4D() );

	Ix = thisMat.Ix * translateMat.Ix + thisMat.Jx * translateMat.Iy + thisMat.Kx * translateMat.Iz + thisMat.Tx * translateMat.Iw;
	Iy = thisMat.Iy * translateMat.Ix + thisMat.Jy * translateMat.Iy + thisMat.Ky * translateMat.Iz + thisMat.Ty * translateMat.Iw;
	Iz = thisMat.Iz * translateMat.Ix + thisMat.Jz * translateMat.Iy + thisMat.Kz * translateMat.Iz + thisMat.Tz * translateMat.Iw;
	Iw = thisMat.Iw * translateMat.Ix + thisMat.Jw * translateMat.Iy + thisMat.Kw * translateMat.Iz + thisMat.Tw * translateMat.Iw;

	Jx = thisMat.Ix * translateMat.Jx + thisMat.Jx * translateMat.Jy + thisMat.Kx * translateMat.Jz + thisMat.Tx * translateMat.Jw;
	Jy = thisMat.Iy * translateMat.Jx + thisMat.Jy * translateMat.Jy + thisMat.Ky * translateMat.Jz + thisMat.Ty * translateMat.Jw;
	Jz = thisMat.Iz * translateMat.Jx + thisMat.Jz * translateMat.Jy + thisMat.Kz * translateMat.Jz + thisMat.Tz * translateMat.Jw;
	Jw = thisMat.Iw * translateMat.Jx + thisMat.Jw * translateMat.Jy + thisMat.Kw * translateMat.Jz + thisMat.Tw * translateMat.Jw;

	Kx = thisMat.Ix * translateMat.Kx + thisMat.Jx * translateMat.Ky + thisMat.Kx * translateMat.Kz + thisMat.Tx * translateMat.Kw;
	Ky = thisMat.Iy * translateMat.Kx + thisMat.Jy * translateMat.Ky + thisMat.Ky * translateMat.Kz + thisMat.Ty * translateMat.Kw;
	Kz = thisMat.Iz * translateMat.Kx + thisMat.Jz * translateMat.Ky + thisMat.Kz * translateMat.Kz + thisMat.Tz * translateMat.Kw;
	Kw = thisMat.Iw * translateMat.Kx + thisMat.Jw * translateMat.Ky + thisMat.Kw * translateMat.Kz + thisMat.Tw * translateMat.Kw;

	Tx = thisMat.Ix * translateMat.Tx + thisMat.Jx * translateMat.Ty + thisMat.Kx * translateMat.Tz + thisMat.Tx * translateMat.Tw;
	Ty = thisMat.Iy * translateMat.Tx + thisMat.Jy * translateMat.Ty + thisMat.Ky * translateMat.Tz + thisMat.Ty * translateMat.Tw;
	Tz = thisMat.Iz * translateMat.Tx + thisMat.Jz * translateMat.Ty + thisMat.Kz * translateMat.Tz + thisMat.Tz * translateMat.Tw;
	Tw = thisMat.Iw * translateMat.Tx + thisMat.Jw * translateMat.Ty + thisMat.Kw * translateMat.Tz + thisMat.Tw * translateMat.Tw;
}

void Mat44::Transpose()
{
	Mat44 mat = Mat44( GetAsFloatArray() );

	Ix = mat.Ix;
	Iy = mat.Jx;
	Iz = mat.Kx;
	Iw = mat.Tx;

	Jx = mat.Iy;
	Jy = mat.Jy;
	Jz = mat.Ky;
	Jw = mat.Ty;

	Kx = mat.Iz;
	Ky = mat.Jz;
	Kz = mat.Kz;
	Kw = mat.Tz;

	Tx = mat.Iw;
	Ty = mat.Jw;
	Tz = mat.Kw;
	Tw = mat.Tw;
}

Mat44 Mat44::GetInvertOrthoNormal() const
{
	Vec3 translation = GetTranslation3D();
	Mat44 inverse = Mat44( GetIBasis4D(), GetJBasis4D(), GetKBasis4D(), GetTranslation4D() );
	inverse.SetTranslation3D( Vec3::ZERO );
	inverse.Transpose();

	Vec3 inverse_translation = inverse.TransformPosition3D( -translation );
	inverse.SetTranslation3D( inverse_translation );

	return inverse;
}

bool Mat44::IsOrthoNormal( const Mat44& mat )
{
	if ( GetInvertOrthoNormal() == mat )
	{
		return true;
	}
	return false;
}

const Mat44 Mat44::CreateXRotationDegrees( float degreesAboutX )
{
	float cosine = CosDegrees( degreesAboutX );
	float sine = SinDegrees( degreesAboutX );

	Mat44 rotationMatrix;
	rotationMatrix.Jy = cosine;
	rotationMatrix.Jz = sine;
	rotationMatrix.Ky = -sine;
	rotationMatrix.Kz = cosine;
	return rotationMatrix;
}

const Mat44 Mat44::CreateYRotationDegrees( float degreesAboutY )
{
	float cosine = CosDegrees( degreesAboutY );
	float sine = SinDegrees( degreesAboutY );

	Mat44 rotationMatrix;
	rotationMatrix.Ix = cosine;
	rotationMatrix.Iz = -sine;
	rotationMatrix.Kx = sine;
	rotationMatrix.Kz = cosine;
	return rotationMatrix;
}

const Mat44 Mat44::CreateZRotationDegrees( float degreesAboutZ )
{
	float cosine = CosDegrees( degreesAboutZ );
	float sine = SinDegrees( degreesAboutZ );

	Mat44 rotationMatrix;
	rotationMatrix.Ix = cosine;
	rotationMatrix.Iy = sine;
	rotationMatrix.Jx = -sine;
	rotationMatrix.Jy = cosine;
	return rotationMatrix;
}

const Mat44 Mat44::CreateTranslationXY( const Vec2& translationXY )
{
	Mat44 rotationMatrix = Mat44::IDENTITY;
	rotationMatrix.Translate2D( translationXY );

	return rotationMatrix;
}

const Mat44 Mat44::CreateTranslation3D( const Vec3& translation3D )
{
	Mat44 rotationMatrix = Mat44::IDENTITY;
	rotationMatrix.Translate3D( translation3D );

	return rotationMatrix;
}

const Mat44 Mat44::CreateUniformScaleXY( float uniformScaleXY )
{
	Mat44 rotationMatrix = Mat44::IDENTITY;
	rotationMatrix.ScaleUniform2D( uniformScaleXY );

	return rotationMatrix;
}

const Mat44 Mat44::CreateNonUniformScaleXY( const Vec2& scaleFactorsXY )
{
	Mat44 rotationMatrix = Mat44::IDENTITY;
	rotationMatrix.ScaleNonUniform2D( scaleFactorsXY );

	return rotationMatrix;
}

const Mat44 Mat44::CreateUniformScale3D( float uniformScale3D )
{
	Mat44 rotationMatrix = Mat44::IDENTITY;
	rotationMatrix.ScaleUniform3D( uniformScale3D );

	return rotationMatrix;
}

const Mat44 Mat44::CreateNonUniformScale3D( const Vec3& scaleFactorsXYZ )
{
	Mat44 rotationMatrix = Mat44::IDENTITY;
	rotationMatrix.ScaleNonUniform3D( scaleFactorsXYZ );

	return rotationMatrix;
}

const Mat44 Mat44::CreateProjectionPerspective( float fovDegrees, float ratio, float nearZ, float farZ )
{
	float height = 1.0f / TanDegrees( fovDegrees * 0.5f ); // how far away are we for the perspective point to be "one up" from our forward line. 
	float zrange = farZ - nearZ;
	float q = 1.0f / zrange;

	float proj[] =
	{
		height / ratio,		0,			0,					0,
		0,					height,		0,					0,
		0,					0,			-farZ * q,			-1,
		0,					0,			nearZ * farZ * q,	0
	};

	return Mat44( proj );
}

const Mat44 Mat44::LookAtRH( Vec3 worldPosition, Vec3 targetPosition, Vec3 worldUp )
{
	Vec3 f = (targetPosition - worldPosition).GetNormalized();
	Vec3 r = CrossProduct3D( f, worldUp ).GetNormalized();
	if ( r == Vec3::ZERO )
	{
		r = CrossProduct3D( f, Vec3(0.f,0.f,1.f) ).GetNormalized();
	}
	Vec3 u = CrossProduct3D( r, f );

	return Mat44( r, u, -f, worldPosition );
}

const Mat44 Mat44::LookAtLH( Vec3 worldPosition, Vec3 targetPosition, Vec3 worldUp )
{
	Vec3 f = -(targetPosition - worldPosition).GetNormalized();
	Vec3 r = CrossProduct3D( f, worldUp ).GetNormalized();
	if( r == Vec3::ZERO )
	{
		r = CrossProduct3D( f, Vec3( 0.f, 0.f, 1.f ) ).GetNormalized();
	}
	Vec3 u = CrossProduct3D( r, f );

	return Mat44( r, u, -f, worldPosition );
}

const Mat44 Mat44::CreateOrthographicProjection( const Vec3& min, const Vec3& max )
{
	// min.x, max.x -> (-1, 1)
	// ndc.x = x - min.x / ( max.x - min.x ) * ( 1.f - -1.f ) + -1
	// mdc.x = x / ( max.x - min.x ) - ( min.x / (max.x - min.x) ) * 2.f + -1.f
	// a = 1.f / (max.x - min.x)
	// b = (-2.f * min.x - max.x + min.x) / (max.x - min.x)
	//	 = -(max.x + min.x) / (max.x - min.x)

	// min.z, max.z -> (0,1)
	// ndc.x = x - min.x / ( max.x - min.x ) * ( 1.f - 0.f ) + -0.f
	// mdc.x = x / ( max.x - min.x ) - ( min.x / (max.x - min.x) ) * 1.f
	// a = 1.f / (max.x - min.x)
	// b = (-min.x) / (max.x - min.x)

	Vec3 diff = max - min;
	Vec3 sum = max + min;

	float mat[] = { 
		2.f / diff.x,		0.f,				0.f,				0.f,
		0.f,				2.f / diff.y,		0.f,				0.f,
		0.f,				0.f,				1.f / diff.z,		0.f,
		-sum.x / diff.x,	-sum.y / diff.y,	-min.z / diff.z,	1.f
	};

	return Mat44( mat );
}

bool Mat44::operator==( const Mat44& compare ) const
{
	const float* myMat  = GetAsFloatArray();
	const float* compareMat  = compare.GetAsFloatArray();
	for ( int i = 0; i < 16 ; i++ )
	{
		if ( myMat[i] != compareMat[i] )
		{
			return false;
		}
	}
	return true;
}