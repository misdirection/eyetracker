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
	CascadeClassifier getFaceCascade(),getEyeCascade();
	bool loadFaceCascade(), loadEyeCascade();

private:
	CascadeClassifier face_cascade,eye_cascade;
};

extern Externals* files;
