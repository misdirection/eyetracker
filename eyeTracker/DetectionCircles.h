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
	bool calculate_distanceBetweenPointsInMatrixAreNearlySame();
	bool setNeighbors(int);
	void calcRotation();

	//getter
	Rect* getCircleArea();
	Point getCoordsOfcircleMatrix(int,int);

	// helper
	bool nearlyEqual(int,int,int);
	vector<Point> circles;
	double distanceOfPoints(Point,Point);
	vector<double> basicAngle;
	// coords of the circleMatrix
	map<int,Point> circleMatrix;
	Rect circleArea;
private:
	Mat* _frame;
	
	Mat _working_frame;
	
	// main values that will be used
	// distance between 2 points in pixels
	vector<double> _distanceBetweenPoints;

	
	// helpers
	vector<double> _distanceBetweenPointsTopResults_temp;
	vector<double> _distanceBetweenPointsInMatrix;
};

