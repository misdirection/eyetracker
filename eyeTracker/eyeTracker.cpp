#include "eyeTracker.h"
#include "fps.h"
#include "ThreadCapturing.h"
#include "ThreadData.h"

VideoCapture* captureDevice;
ThreadData *datas;
map<int,ThreadCapturing*> devices;

eyeTracker::eyeTracker(void){}
eyeTracker::~eyeTracker(void){}

//counts attached devices and returns the amount of devices
int getCountOfAttachedCaptureDevices()
{
	int number = 0;
	while (true)
	{
		captureDevice->open(number);
		if (!captureDevice->isOpened())
		{
			break;
		}
		else
		{
			devices[number]=nullptr;
			number++;
			captureDevice->release();
		}
	}
	return number;
}

void switchDevices(int number)
{
	if (devices[number]==nullptr)
	{
	stringstream text;
    text<< "web cam device " << number; 
    string str1= text.str();
	LPSTR s = const_cast<char *>(str1.c_str());
	devices[number]=new ThreadCapturing(s,*captureDevice, number,datas->getThreadId());
	devices[number]->Start();
	}
	else
	{
	devices[number]->Stop();
	delete devices[number];
	devices[number]=nullptr;
	}
}
int main( int argc, const char** argv ) 
{
	// get count of recognized cameras
    captureDevice = new VideoCapture;
	datas = new ThreadData();
	
	int count = getCountOfAttachedCaptureDevices();
	int number=-1;
	if (count > 0) 
	{
		cout << "Available devices: " << count << endl << "Press number between 0 and " << count-1 << " to start first camera";
		while(number<0 || number >=count)
		{
		while(!_kbhit());
		string temp; temp= getch(); number = atoi(temp.c_str());
		}
		switchDevices(number);
	}
	int key;
	while((key = cvWaitKey(1)) != 0x1b)
    {
		switch(key)
        {
			case '0': cout << "TEST";break;
			case '1': cout << "TEST";break;
		}
	}
	//if( !captureDevice->open(0)){ printf("none\n");return -1; } //checks if a device is found
    
	//ThreadCapturing *webcam;
    //char windowName[256];
    //sprintf_s(windowName, "Face Tracker Window webcam");
	//webcam = new ThreadCapturing(windowName,*captureDevice, 0,datas->getThreadId());
	//webcam->Start();
	
	//while(webcam->isRunning())
	/*while(1)
	{
	}
	*/
    //delete captureDevice;
	//captureDevice=nullptr;
	return (0);
}
