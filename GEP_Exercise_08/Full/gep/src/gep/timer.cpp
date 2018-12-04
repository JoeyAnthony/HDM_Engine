#include "stdafx.h"
#include "gep/timer.h"
#include "gep/exception.h"
#include <Windows.h>

gep::Timer::Timer()
{
	//timer starts on construction
	QueryPerformanceCounter(&performanceCount);
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	resolution = 1.0 / (double)frequency.QuadPart;
}

float gep::Timer::getTimeAsFloat() const
{
	float t = calcTime() * resolution;
    return t;
}

double gep::Timer::getTimeAsDouble() const
{
    return calcTime() * resolution;
}

gep::uint64 gep::Timer::getTime() const
{
	unsigned long long t = calcTime();
	return t * ceil(resolution);
}

void gep::Timer::pause()
{
	if (isPaused)
		return;
	timeValue = calcTime();
	isPaused = true;
}

void gep::Timer::unpause()
{
	if (!isPaused)
		return;
	//reset performancecounter
	QueryPerformanceCounter(&performanceCount);
	isPaused = false;
}

double gep::Timer::calcTime() const
{
	if (isPaused)
		return timeValue;
	//get start time and current time
	LARGE_INTEGER performanceCountNew;
	QueryPerformanceCounter(&performanceCountNew);
	double lastCount = (double)performanceCount.QuadPart;
	double newCount = (double)performanceCountNew.QuadPart;

	//time between start and current
	double delta = newCount - lastCount;
	//add to resulting time

	return timeValue + delta;
}

gep::PointInTime::PointInTime()
{
	Timer timer;
	timeValue = timer.timeValue;
	resolution = timer.resolution;
}

gep::PointInTime::PointInTime(Timer& timer)
{
	resolution = timer.resolution;
	timeValue = timer.calcTime();
}

float gep::PointInTime::operator - (const PointInTime& rh) const
{
	return (timeValue - rh.timeValue) * resolution;
}

bool gep::PointInTime::operator < (const PointInTime& rh) const
{
	return timeValue < rh.timeValue ? true : false;
}

bool gep::PointInTime::operator > (const PointInTime& rh) const
{
    return timeValue > rh.timeValue ? true : false;
}

bool gep::PointInTime::operator <= (const PointInTime& rh) const
{
    return timeValue <= rh.timeValue ? true : false;
}

bool gep::PointInTime::operator >= (const PointInTime& rh) const
{
    return timeValue >= rh.timeValue ? true : false;
}

bool gep::PointInTime::operator == (const PointInTime& rh) const
{
    return timeValue == rh.timeValue ? true : false;
}

bool gep::PointInTime::operator != (const PointInTime& rh) const
{
    return timeValue != rh.timeValue ? true : false;
}
