#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"
#include <windows.h>
#include <conio.h>

using namespace std;
using namespace cv;

class Externals
{
public:
	Externals(void);
	~Externals(void);
	//CascadeClassifier getFaceCascade(),getEyeCascade();
	bool loadFaceCascade(), loadEyeCascade();
	FileStorage getfs();
	FileStorage getCalibFile();
	bool loadCalibFile();
	void setFocalLengths(double);
	vector<double>getFocalLengths();
	FileNode getFaceCascade(),getEyeCascade();

private:
	CascadeClassifier face_cascade,eye_cascade;
	FileStorage fs, storageFace, storageEye;
	vector<double> focalLengths;
};

class DeviceInformation
{
public:
	DeviceInformation(void);
	~DeviceInformation(void);
	HRESULT EnumerateDevices(REFGUID, IEnumMoniker **);
	void DisplayDeviceInformation(IEnumMoniker *);
	void DeviceInformation::getData();

	string getName(int i);
	string getPath(int i);


private:
	vector<string> _name;
	vector<string> _path;
};

extern Externals* files;
extern DeviceInformation* captureDeviceInfo;