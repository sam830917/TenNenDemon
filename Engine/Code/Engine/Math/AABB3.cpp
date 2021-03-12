#include "AABB3.hpp"

AABB3::AABB3( const AABB3& copyFrom )
	:mins(copyFrom.mins),
	maxs(copyFrom.maxs)
{
}

AABB3::AABB3( const Vec3& inialMins, const Vec3& inialMaxs )
	:mins(inialMins),
	maxs(inialMaxs)
{
}

const Vec3 AABB3::GetCenter() const
{
	return Vec3( ( mins.x + maxs.x ) * 0.5f, ( mins.y + maxs.y ) * 0.5f, ( mins.z + maxs.z ) * 0.5f );
}

void AABB3::SetCenter( const Vec3& newCenter )
{
	Vec3 oldCenter = GetCenter();
	Translate( newCenter - oldCenter );
}

void AABB3::Translate( const Vec3& translation )
{
	mins += translation;
	maxs += translation;
}
