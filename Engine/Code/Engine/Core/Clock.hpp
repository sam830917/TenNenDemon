#pragma once
#include <vector>

class Clock
{
public:
	Clock();
	Clock( Clock* parentClock );
	~Clock();

	void Update( double dt );
	void Reset();

	void SetScale( float scale ) { m_scale = scale; }
	void Pause() { m_isPaused = true; }
	void UnPause() { m_isPaused = false; }

	double	GetTotalTime() const { return m_totalTime; }
	double	GetFrameTime() const { return m_frameTime; }
	float	GetScale() const { return m_scale; }
	bool	IsPaused() const { return m_isPaused; }

	double  GetTotalElapsedSeconds() const { return m_totalTime; }
	double	GetLastDeltaSeconds() const { return m_lastFrameTime; }

public:
	static Clock*	GetMaster();
	static void		SystemStartup();

private:

private:
	bool	m_isPaused = false;
	float	m_scale = 1.f;
	double	m_totalTime = 0.0;
	double	m_frameTime = 0.0;
	double	m_lastFrameTime = 0.0;

	Clock* m_parentClock = nullptr;
	std::vector<Clock*> m_childClocks;

};