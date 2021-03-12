#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Math/MathUtils.hpp"

AnalogJoystick::AnalogJoystick( float innerDeadZoneFraction, float outerDeadZoneFraction )
	:m_innerDeadZoneFraction(innerDeadZoneFraction),
	m_outerDeadZoneFraction(outerDeadZoneFraction)
{
}

void AnalogJoystick::Reset()
{
	m_rawPosition = Vec2( 0.f, 0.f );
}

void AnalogJoystick::UpdatePosition( float rawNormalizedX, float rawNormalizedY )
{
	Vec2 rawNormalizedVec2 = Vec2( rawNormalizedX, rawNormalizedY );
	float rawMagnitude = rawNormalizedVec2.GetLength();
	float degrees = rawNormalizedVec2.GetAngleDegrees();
	if ( rawMagnitude < m_innerDeadZoneFraction )
	{
		rawNormalizedVec2 = Vec2();
	}
	else if ( rawMagnitude > m_outerDeadZoneFraction )
	{
		rawNormalizedVec2.SetLength( 1.f );
	}
	else
	{
		rawNormalizedVec2 *= ((rawMagnitude - m_innerDeadZoneFraction - ( 1.f - m_outerDeadZoneFraction )) / (1.f - m_innerDeadZoneFraction - ( 1.f - m_outerDeadZoneFraction )));
	}

	m_correctedPosition = rawNormalizedVec2;
	m_correctedDegrees = degrees;
	m_correctedMagnitude = rawNormalizedVec2.GetLength();
}
