#include "stdafx.h"
#include "gep/timer.h"
#include "gep/exception.h"
#include <Windows.h>

gep::Timer::Timer()
{
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter;
}

float gep::Timer::getTimeAsFloat() const
{
    return 0.0f;
}

double gep::Timer::getTimeAsDouble() const
{
    return 0.0;
}

gep::uint64 gep::Timer::getTime() const
{
    return 0;
}

void gep::Timer::pause()
{
}

void gep::Timer::unpause()
{
}

gep::PointInTime::PointInTime(Timer& timer)
{
	this->timer = timer;
}

float gep::PointInTime::operator - (const PointInTime& rh) const
{
   return 0.0f;
}

bool gep::PointInTime::operator < (const PointInTime& rh) const
{
    return false;
}

bool gep::PointInTime::operator > (const PointInTime& rh) const
{
    return false;
}

bool gep::PointInTime::operator <= (const PointInTime& rh) const
{
    return false;
}

bool gep::PointInTime::operator >= (const PointInTime& rh) const
{
    return false;
}

bool gep::PointInTime::operator == (const PointInTime& rh) const
{
    return false;
}

bool gep::PointInTime::operator != (const PointInTime& rh) const
{
    return false;
}
