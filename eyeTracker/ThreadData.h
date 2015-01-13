#pragma once
#include <iostream>
#include "eyeTracker.h"
#include "Thread.h"

class ThreadData : public Thread
{
public:
	ThreadData(void);
	~ThreadData(void);
	void Run();

private:
	void drawCalibCircle(int);
	int dim1,dim2;
	Mat base;
	int calibprocess;
	vector<int> receivedThreads;
	map<int,map<int,map<int,vector<Point>>>> calib_tmp;
	map<int,map<int,map<int,Point>>> calib;
	//map<int,int,int> calibValues; // first=threadID, second=x, third=y;
};

