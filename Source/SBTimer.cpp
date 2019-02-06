#include "SBTimer.h"
#include <Windows.h>
#include "JuceHeader.h"

SBTimer* SBTimer::g_pTimerInstance = nullptr;

SBTimer::SBTimer()
{
	int64_t freq;
	QueryPerformanceFrequency((PLARGE_INTEGER)&freq);
	m_fFreq = (double)freq;
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
	QueryPerformanceCounter((PLARGE_INTEGER)&m_nStartTime);
}

double SBTimer::getTime()
{
	int64_t endTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&endTime);
	return (double)(endTime - m_nStartTime) / m_fFreq;
}

void SBTimer::update(double newTime)
{
	fFrameTime = newTime - fTime;

	if (fFrameTime < 0.0)
		fFrameTime = 0.0;

	fTime = newTime;
	fCounter += fFrameTime;
}
