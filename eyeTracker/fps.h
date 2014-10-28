#pragma once
#include <thread>
using namespace std::chrono; // fps calculation
#include <math.h>


class fps
{
public:
	fps(void);
	~fps(void);
	
	void setStart();
	high_resolution_clock::time_point const getStart();
	void setEnd();
	high_resolution_clock::time_point const getEnd();
	double const getFPS();


private:
	high_resolution_clock::time_point _start,_end;
	double _framesPerSecond;
};

