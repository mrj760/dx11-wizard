#include "Timer.h"

Timer::Timer()
{
	start = high_resolution_clock::now();
	stop = high_resolution_clock::now();
}

double Timer::elapsedMS()
{
	if (isRunning)
	{
		auto elapsed = duration<double, std::milli>(high_resolution_clock::now() - start);
		return elapsed.count();
	}
	else 
	{
		auto elapsed = duration<double, std::milli>(stop - start);
		return elapsed.count();
	}
}

void Timer::restartTimer()
{
	isRunning = true;
	start = high_resolution_clock::now();
}

bool Timer::stopTimer()
{
	if (!isRunning)
		return false;

	stop = high_resolution_clock::now();
	isRunning = false;
	return true;
}

bool Timer::startTimer()
{
	if (isRunning)
		return false;
	
	isRunning = true;
	start = high_resolution_clock::now();
	return true;
}
