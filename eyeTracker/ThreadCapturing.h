#pragma once
#include <iostream>
#include "eyeTracker.h"


class ThreadCapturing{

public:
	ThreadCapturing(void);
	~ThreadCapturing(void);
	ThreadCapturing(LPSTR windowName, VideoCapture captureDevice, DWORD dataThreadID);

	static unsigned long __stdcall CaptureThread(void*);
	bool StartCapture();
	void Run();
	void StopCapture();
	DWORD getThreadId();

private: 	
	char _windowName[256];
	VideoCapture _captureDevice;
	HANDLE _hThread;
	DWORD _threadId, _dataThreadId;
	bool _running;
};
