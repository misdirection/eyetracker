#pragma once
#include "eyeTracker.h"
#include "fps.h"
#include "ThreadCapturing.h"
#include "ThreadData.h"
#include "calib.h"
#include <fstream>
#include <fstream>

VideoCapture* captureDevice = new VideoCapture;
// data thread
ThreadData *datas = new ThreadData;
// capture threads in a map for easy access
map<int,ThreadCapturing*> devices;
Externals* files = new Externals;
DeviceInformation* captureDeviceInfo = new DeviceInformation;

//counts attached devices and returns the amount of devices
bool setup()
{
	// attached devices are checked and device informations will be available
	// check devices from 0 to x. If available, then add value to the vector with null pointer
	int number=0;
	while (number>=0)
	{
		captureDevice->open(number);
		if (!captureDevice->isOpened())
		{
			//no device -> return unsuccessful setup;
			//when all devices recognized, go out of while loop
			if (number == 0) {cout << "no available devices found\n"; return false;}
			number=-1;
		}
		else
		{
			//test cout for the path and name of the device (accessable everywhere due to extern)
			// use this to check if there is a calib file already
			//cout << captureDeviceInfo->getName(number) << endl;			
			//cout << captureDeviceInfo->getPath(number) << endl;			
			devices[number]=nullptr;
			number++;
			captureDevice->release();
		}
	}
	// loading cascade files
	// allocating space for the Externals class
	if (!files->loadFaceCascade()) {cout << "no haar cascade file for face recognition found\n"; return false;}
	if (!files->loadEyeCascade()) {cout << "no haar cascade file for eye recognition found\n"; return false;}
	if (!files->loadCalibFile()){cout << "Could not open the configuration file: \"" << "default.xml" << "\"" << endl; return false;}
	if (!fs.isOpened())
	{
		cout << "Could not open the configuration file: \"" << inputSettingsFile << "\"" << endl;
		return -1;
	}
	return true;
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
	cout << "Welcome to the Eyetracker9000!\n-----------------------------------------------------------------" << endl;
	if (!setup()) {cout << "execution will be stopped due to errors.\n";system("pause");return -1;}
	cout << "Starting calibration for all available devices ("<<devices.size() << ")..."<< endl;
	//Callibration
	for(int i =0; i <devices.size();i++)
	{
		ifstream input;
		string str;
		input.open(captureDeviceInfo->getName(i)+".txt");
		if(!input.is_open())
		{	
			cout << "Can't find an existing file, calibration starts for " << captureDeviceInfo->getName(i) <<"!" <<  endl;
			startCalibration(i);
			cvDestroyWindow("Image View");
		}
		else
		{
			double focalLength;
			cout << "loading calibrationfile for " << captureDeviceInfo->getName(i) <<"!" <<  endl;
			getline(input, str);
			focalLength =stod(str);
			files->setFocalLengths(focalLength);
			//dist_calc(focalLength);
			//cout << "Focal Length"<<focalLength<<endl;

		}
	}

	cout << "Calibration of all devices done!\n-----------------------------------------------------------------" << endl;
	cout << "Starting the eyetracker...." << endl;
	//Eyetracking
	cout << "Available devices: " << devices.size() << endl << "Press number between 0 and " << devices.size()-1 << " to start first camera.";
	// until you select an available device
	int number=-1;
	while(number<0 || number >=devices.size() )
	{
		while(!_kbhit());
		string temp; temp= getch(); number = atoi(temp.c_str());
		if (number == 0 && !(temp=="0")) {number=-1;}
	}
	// activate the selected device
	switchDevices(number);

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
	return (0);
}