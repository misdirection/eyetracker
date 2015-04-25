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

	int detectEye1(Mat&,vector<Mat>&,vector<Rect>&);  // Function to detect user's face and eye
	void trackEye1(Mat&,vector<Mat>&,vector<Rect>&); // Function to track user's eye given its template
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
	// confirm pupils after getting circlesGrid
private:
    vector<Mat> eye_tpl;  // The eye template
    vector<Rect> eye_bb;  // The eye bounding box

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

