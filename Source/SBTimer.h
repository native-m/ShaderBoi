#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

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
	double m_nStartTime;
	double m_fFreq;
};