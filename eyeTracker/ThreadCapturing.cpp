#include "ThreadCapturing.h"
#include "fps.h"
#include "BaseDetection.h"
ThreadCapturing::ThreadCapturing(void){};
ThreadCapturing::~ThreadCapturing(void){
	destroyWindow(_windowName);
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
	namedWindow(_windowName, CV_WINDOW_AUTOSIZE);
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
	BaseDetection Face;
	while(_running)
	{
		Mat frame;
		_captureDevice >> frame; // get a new frame from camera

		Face.detect(frame);
		CvFont font;
		std::stringstream text;
		text<< framesPerSeconds.getFPS(); 
		String str1= text.str();
		putText(frame,str1,cvPoint(30,30), FONT_HERSHEY_SIMPLEX,1,Scalar(255,255,0),1,8,false);
		imshow(_windowName, Face.drawRect(frame)); //displays an image in the specified window
		//cout << "fps:" << framesPerSeconds.getFPS() << endl;
		//if(cvWaitKey(1) >= 0);
	}

}
