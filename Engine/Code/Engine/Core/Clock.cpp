#include "Engine/Core/Clock.hpp"

Clock* g_masterClock = nullptr;

Clock::Clock()
{
	if ( g_masterClock )
	{
		m_parentClock = g_masterClock;
		g_masterClock->m_childClocks.push_back( this );
	}
}

Clock::Clock( Clock* parentClock )
{
	if( parentClock )
	{
		m_parentClock = parentClock;
		parentClock->m_childClocks.push_back( this );
	}
	else
	{
		m_parentClock = g_masterClock;
		g_masterClock->m_childClocks.push_back( this );
	}
}

Clock::~Clock()
{
	for( int i = 0; i < (int)m_childClocks.size(); ++i )
	{
		if( m_parentClock )
		{
			m_childClocks[i]->m_parentClock = m_parentClock;
		}
		else
		{
			m_parentClock = m_childClocks[i];
		}
	}

	if ( m_parentClock )
	{
		for( int i = 0; i < (int) m_parentClock->m_childClocks.size(); ++i )
		{
			if ( this == m_parentClock->m_childClocks[i] )
			{
				m_parentClock->m_childClocks[i] = nullptr;
			}
		}
	}
}

void Clock::Update( double dt )
{
	if( IsPaused() )
	{
		dt = 0.0;
	}
	else
	{
		dt *= m_scale;
	}

	// update my clock concept of time
	m_lastFrameTime = m_frameTime;
	m_frameTime = dt;
	m_totalTime += dt;

	for ( int i = 0; i < (int) m_childClocks.size(); ++i )
	{
		m_childClocks[i]->Update( dt );
	}
}

void Clock::Reset()
{
	m_totalTime = 0.0;
}

Clock* Clock::GetMaster()
{
	return g_masterClock;
}

void Clock::SystemStartup()
{
	g_masterClock = new Clock();
	g_masterClock->Reset();
}
