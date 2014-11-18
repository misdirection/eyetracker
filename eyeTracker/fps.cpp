#include "fps.h"

fps::fps(void) : _start(high_resolution_clock::now()),_end(),_framesPerSecond(0)
{
}


fps::~fps(void)
{
}

// sets _start to the beginning of the fps count
void fps::setStart()
{
	_start=high_resolution_clock::now();
}

//returns start value of the time when the fps count starts
high_resolution_clock::time_point const fps::getStart()
{
	return _start;
}

// sets _end to the end of the fps count
void fps::setEnd()
{
	_end=high_resolution_clock::now();
}
//returns _end value of the time when the fps count end
high_resolution_clock::time_point const fps::getEnd()
{
	return _end;
}

// getFPS will count the elapsed time of a run and reset the count start for next frame
// returns double value for frames
double const fps::getFPS()
{
	_end=high_resolution_clock::now();
	_framesPerSecond=floor(1000.0/duration_cast<milliseconds>(_end - _start).count()*100)/100;
	_start=high_resolution_clock::now();
	return _framesPerSecond;
}