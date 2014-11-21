#include "eyeTracker.h"
#include "fps.h"
#include "ThreadCapturing.h"
#include "ThreadData.h"

VideoCapture* captureDevice;
// data thread
ThreadData *datas;
// capture threads in a map for easy access
map<int,ThreadCapturing*> devices;


eyeTracker::eyeTracker(void){}
eyeTracker::~eyeTracker(void){}

//counts attached devices and returns the amount of devices
void getCountOfAttachedCaptureDevices()
{
	int number=0;
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
}

void switchDevices(int number)
{
	if (number<devices.size())
	{
		if (devices[number]==nullptr)
		{
			stringstream text;
			text<< "web cam device " << number; 
			string str1= text.str();
			devices[number]=new ThreadCapturing(const_cast<char *>(str1.c_str()),*captureDevice, number,datas->getThreadId());
			devices[number]->Start();
		}
		else
		{
			devices[number]->Stop();
			delete devices[number];
			devices[number]=nullptr;
		}
	}
}
int main( int argc, const char** argv ) 
{
	// get count of recognized cameras
	captureDevice = new VideoCapture;
	datas = new ThreadData();
	
	getCountOfAttachedCaptureDevices();
	int number=-1;
	
	if (devices.size() > 0) 
	{
		cout << "Available devices: " << devices.size() << endl << "Press number between 0 and " << devices.size()-1 << " to start first camera";
		while(number<0 || number >=devices.size() )
		{
		while(!_kbhit());
		string temp; temp= getch(); number = atoi(temp.c_str());
		}
		switchDevices(number);
	}
	char key;
	
while((key = cvWaitKey(0)) != 0x1b)
    {

		switch(key)
        {
			case '0': switchDevices(0); break;
			case '1': switchDevices(1); break;
			case '2': switchDevices(2); break;
			case '3': switchDevices(3); break;
			case '4': switchDevices(4); break;
			case '5': switchDevices(5); break;
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
