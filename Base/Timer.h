#pragma once
#include <chrono>

using namespace std::chrono;

class Timer
{
public:

	Timer();
	double elapsedMS(); // return elapsed ms since last call
	void restartTimer(); // reset the timer
	bool stopTimer(); // freeze the timer
	bool startTimer(); // start the timer

private:

	bool isRunning = false;

	#ifdef _WIN32
		time_point<steady_clock> start;
		time_point<steady_clock> stop;
	#else // different for linux (directx with linux?)
		time_point<system_clock> start;
		time_point<system_clock> stop;
	#endif
};

