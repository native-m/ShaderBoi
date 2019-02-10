#include "SBTimer.h"

SBTimer* SBTimer::g_pTimerInstance = nullptr;

SBTimer::SBTimer()
{
	fTime = 0.0;
	fFrameTime = 0.0;
	fCounter = 0.0;
	reset();
}

SBTimer::~SBTimer()
{
}

void SBTimer::reset()
{
	m_nStartTime = Time::getMillisecondCounterHiRes();
}

double SBTimer::getTime()
{
	double endTime = Time::getMillisecondCounterHiRes();
	return (endTime - m_nStartTime) * 0.001 ;
}

void SBTimer::update(double newTime)
{
	fFrameTime = newTime - fTime;

	if (fFrameTime < 0.0)
		fFrameTime = 0.0;

	fTime = newTime;
	fCounter += fFrameTime;
}
