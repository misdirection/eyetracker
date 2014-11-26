#pragma once
#include "Thread.h"

class DetectionCircles
{
public:
	DetectionCircles(void);
	~DetectionCircles(void);
	void detect(Mat*, Rect*);
	bool calculateCircleArea(Rect*);
	bool detectCircles();
	void calculateDistanceBetweenPoints();
	void fillCircleMatrix();
	int getNeighborInMatrix(int,int);
	bool calc3rdOutOf2(int,int,int);
	//getter
	Rect* getCircleArea();

	// helper
	bool nearlyEqual(int,int);
	vector<Point> circles;
	double distanceOfPoints(Point,Point);
	map<int,Point> circleMatrix;

private:
	Mat* _frame;
	Rect circleArea;
	Mat _working_frame;
	double _distanceBetweenPoints;
	vector<double> _distanceBetweenPointsTopResults_temp;
	
};

