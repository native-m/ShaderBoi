#pragma once

#include <cstdint>

class SBTimer
{
public:
	SBTimer();
	~SBTimer();

	void reset();
	double getTime();
	void update(double newTime);
	inline const double getFps() { return 1.0 / fFrameTime; }

	static SBTimer* g_pTimerInstance;

public:
	double fTime;
	double fFrameTime;
	double fCounter;

private:
	int64_t m_nStartTime;
	double m_fFreq;
};