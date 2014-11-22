#pragma once
#include "eyeTracker.h"



class BaseDetection
{
public:
	//c'tors
	BaseDetection();

	//d'tor
	~BaseDetection(void);

	//Methods
	Mat detectAndDrawEllipse(Mat frame);
	Mat detectAndDrawRect( Mat frame );
private:

	CascadeClassifier face_cascade, eye_cascade;
};
