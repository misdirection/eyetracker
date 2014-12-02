// Detection class for face and eyes
#pragma once
#include "eyeTracker.h" // access of extern global Externals
#include "Thread.h"

class DetectionBasic
{
public:
	DetectionBasic(void);
	~DetectionBasic(void);

	void detect(Mat*); // basic execution of the instance for all detections here
	bool detectFace(); // resize and detect the face
	void calculateEyeAreas(); // area where the eye can only be
	void detectEye();

	// getter for values
	// get pointer to the created rectangle
	Rect* getFaceRect();
	Rect getEyeRect(int); // int=0 is left eye, int=1 is right eye
	vector<Rect> getAllEyeRect(){return eyes;};
	Mat* getFrame(){return _frame;};
	//helper functions
	void setRectangleZero(Rect);
	bool isRectangleZero(Rect);
	bool nearlyEqual(int,int);
	
private:
	Mat* _frame; 
	Mat _working_frame;
	Rect face;
	vector<Rect> eyes;
};

