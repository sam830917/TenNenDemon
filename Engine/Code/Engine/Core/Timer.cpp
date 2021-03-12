#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"

Timer::Timer()
{
}

Timer::Timer( Clock* clock )
{
	m_clock = clock;
}

Timer::~Timer()
{
}

void Timer::SetSeconds( Clock* clock, double timeToWait )
{
	m_clock = clock;
	m_startSeconds = clock->GetTotalTime();
	m_durationSeconds = timeToWait;
}

void Timer::SetSeconds( double timeToWait )
{
	if ( m_clock == nullptr )
	{
		m_clock = Clock::GetMaster();
	}
	m_startSeconds = m_clock->GetTotalTime();
	m_durationSeconds = timeToWait;
}

void Timer::Reset()
{
	m_startSeconds = m_clock->GetTotalTime();
}

void Timer::Stop()
{
	m_durationSeconds = -1.0;
}

double Timer::GetElapsedSeconds() const
{
	if( !HasElapsed() )
	{
		return 0.0;
	}
	double currentTime = m_clock->GetTotalTime();
	return m_startSeconds - currentTime;
}

double Timer::GetSecondsRemaining() const
{
	if ( HasElapsed() )
	{
		return 0.0;
	}
	double currentTime = m_clock->GetTotalTime();
	return currentTime - m_startSeconds;
}

bool Timer::HasElapsed() const
{
	double currentTime = m_clock->GetTotalTime();
	return (currentTime > (m_startSeconds + m_durationSeconds));
}

bool Timer::CheckAndDecrement()
{
	if( HasElapsed() )
	{
		// remove an interval of time
		m_startSeconds += m_durationSeconds;
		return true;
	}
	else
	{
		return false;
	}
}

bool Timer::CheckAndReset()
{
	if( HasElapsed() )
	{
		// remove an interval of time
		m_startSeconds = m_clock->GetTotalTime();
		return true;
	}
	else
	{
		return false;
	}
}

bool Timer::IsRunning() const
{
	return !HasElapsed();
}
