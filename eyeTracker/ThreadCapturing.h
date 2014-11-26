#pragma once
#include <iostream>
#include "eyeTracker.h"
#include "Thread.h"

class ThreadCapturing : public Thread{

public:
	ThreadCapturing(void);
	~ThreadCapturing(void);
	ThreadCapturing(LPSTR, VideoCapture, int, DWORD);
	void Run();
	
	
private: 	
	char _windowName[256];
	VideoCapture _captureDevice;
	DWORD _dataThreadId;
	int _deviceNumber;
};
