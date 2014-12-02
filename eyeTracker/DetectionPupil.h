#pragma once
#include "DetectionBasic.h"
#include <queue>
class DetectionPupil
{
public:
	DetectionPupil(void);
	~DetectionPupil(void);

	Point findEyeCenter(Mat*,Rect);
	Point detectPupil(Rect, Mat*);
	void scaleToFastSize(const Mat&,Mat&);
	Mat floodKillEdges(Mat&);
	bool floodShouldPushPoint(const Point&, const Mat&);
	bool inMat(Point,int,int);
	Point unscalePoint(Point, Rect);
	void testPossibleCentersFormula(int, int, unsigned char,double, double,Mat&);
	Mat computeMatXGradient(const Mat &);
	Mat matrixMagnitude(const Mat&, const Mat&);
	double computeDynamicThreshold(const Mat&, double);


private:
	Mat _working_frame;

};

