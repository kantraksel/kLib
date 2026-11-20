// kLib: Last changed 27.08.2026
#include <chrono>
#include <thread>
#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include "Time.hpp"

#if _WIN32
// guaranteed 1MHz+ (<1us)
static long long GetFrequency()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	return freq.QuadPart;
}

static long long Frequency = GetFrequency();
static long long FrequencyMilli = Frequency / std::milli::den;
static double TickTimeMilli = 1.0 / double(FrequencyMilli);

double Time::SteadyNow()
{
	LARGE_INTEGER ctr;
	QueryPerformanceCounter(&ctr);
	return double(ctr.QuadPart) * TickTimeMilli;
}

long long Time::SteadyNowInt()
{
	LARGE_INTEGER ctr;
	QueryPerformanceCounter(&ctr);
	return ctr.QuadPart / FrequencyMilli;
}
#else
double Time::SteadyNow()
{
	std::chrono::duration<double, std::milli> now = std::chrono::steady_clock::now().time_since_epoch();
	return now.count();
}

long long Time::SteadyNowInt()
{
	auto now = std::chrono::steady_clock::now().time_since_epoch();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}
#endif

void Time::Sleep(unsigned int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
