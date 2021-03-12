#pragma once

class Clock;

class Timer
{
public:
	Timer();
	Timer( Clock* clock );
	~Timer();
	void SetSeconds( Clock* clock, double timeToWait );    // sets and resets timer, and clock
	void SetSeconds( double timeToWait );                  // keeps current clock, or sets to Master if no clock has been set yet

	void Reset();                                   // reset the timer, but it keeps running

	void Stop();                                    // stops the timer

	double GetElapsedSeconds() const;               // return amount of time accrued on this timer
	double GetSecondsRemaining() const;             // returns amount of time until HasElapsed() will return true (0 or negative numbers means it has already elapsed)

	bool HasElapsed() const;                        // timer has elapsed the timer
	bool CheckAndDecrement();                       // if has elapsed, removes one interval of time and returns true, otherwise returns false
	int  CheckAndDecrementAll();                    // returns how many intervals have passed and removes them  
	bool CheckAndReset();                           // if has elapsed, resets and returns true, otherwise returns false; 

	bool IsRunning() const;                         // timer is accruing time


private:
	Clock* m_clock = nullptr; // clock timer is based off of

	int m_interval = 0;
	double m_startSeconds    =  0.0;
	double m_durationSeconds = -1.0;    // negative means stopped

};