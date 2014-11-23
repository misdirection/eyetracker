#pragma once
#include "Thread.h"
class BaseDetection
{
public:
	//c'tors
	BaseDetection();

	//d'tor
	~BaseDetection(void);

	//Methods
	vector<cv::Rect> detect(Mat frame);
	Mat BaseDetection::drawEllipse(Mat);
	Mat BaseDetection::drawRect(Mat);

private:
	CascadeClassifier face_cascade, eye_cascade;
	vector<cv::Rect> faces;
};
