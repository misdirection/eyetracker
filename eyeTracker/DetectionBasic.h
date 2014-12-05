// Detection class for face and eyes
#pragma once
#include "eyeTracker.h" // access of extern global Externals
#include "Thread.h"
#include "DetectionPupil.h"

class DetectionBasic
{
public:
	DetectionBasic(void);
	~DetectionBasic(void);

	void detect(Mat*); // basic execution of the instance for all detections here
	bool detectFace(); // resize and detect the face
	void calculateEyeAreas(); // area where the eye can only be
	void detectEye();
	void detectPupils();

	// getter for values
	// get pointer to the created rectangle
	Rect* getFaceRect();
	Rect* getEyeRect(int); // int=0 is left eye, int=1 is right eye
	Point* getPupilPoint(int);
	//helper functions
	void setRectangleZero(Rect);
	bool isRectangleZero(Rect);
	bool nearlyEqual(int,int,int);
private:
	Mat* _frame; 
	Mat _working_frame;
	Rect face;
	vector<Rect> eyes;
	vector<Point> pupil;
	DetectionPupil detPup;
	double _distanceOfPupils;
	int _distanceOfPupilsCounter;
	CascadeClassifier _face_cascade,_eye_cascade;
};

