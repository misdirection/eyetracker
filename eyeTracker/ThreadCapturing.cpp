#include "ThreadCapturing.h"
#include "fps.h"

ThreadCapturing::ThreadCapturing(void){};
ThreadCapturing::~ThreadCapturing(void){	
	delete data;
	data = nullptr;};
ThreadCapturing::ThreadCapturing(LPSTR windowName, VideoCapture captureDevice,int deviceNumber, DWORD dataThreadID) :
	Thread(), 
	_captureDevice(captureDevice),
	_dataThreadId(dataThreadID)
{
	data=new dataPackage;
	strcpy_s(_windowName, windowName);
	_captureDevice.open(deviceNumber);
}






// main method of the ThreadCapturing which runs after the StartCapture
void ThreadCapturing::Run()
{
	data->y=111;
	data->x=GetCurrentThreadId();

	// test of  transfer to data thread only string here, but can be used with anything else
	// send message of the thread to data thread
	ThreadCom_send(_dataThreadId,(WPARAM)data);
	fps framesPerSeconds;
	while(_running)
	{
		Mat frame;
		_captureDevice >> frame; // get a new frame from camera
		imshow(_windowName, frame); //displays an image in the specified window
		cout << "fps:" << framesPerSeconds.getFPS() << endl;
		if(waitKey(30) >= 0) break ;
	}

}
