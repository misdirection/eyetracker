#include "eyeTracker.h"
#include "fps.h"
#include "ThreadCapturing.h"
#include "ThreadData.h"


eyeTracker::eyeTracker(void){}
eyeTracker::~eyeTracker(void){}

//counts attached devices and returns the amount of devices
int getCountOfAttachedCaptureDevices()
{
	int number = 0;
	while (true)
	{
		VideoCapture cap;
		cap.open(number);
		if (!cap.isOpened())
		{
			break;
		}
		else
	{
			number++;
			cap.release();
		}
	}
	return number;
}



int main( int argc, const char** argv ) 
{
    // get count of recognized cameras
    int count = getCountOfAttachedCaptureDevices();
    VideoCapture captureDevice;
    if( !captureDevice.open(0)){ printf("none\n");return -1; } //checks if a device is found
    ThreadCapturing *webcam;
    char windowName[256];
    sprintf_s(windowName, "Face Tracker Window webcam");
    ThreadData *datas = new ThreadData();
	webcam = new ThreadCapturing(windowName,captureDevice, datas->getThreadId());
    webcam->StartCapture();
	while(int key = cvWaitKey(0) != 0x1b)
	{

	}
	
//    system("pause");
    return (0);
}
