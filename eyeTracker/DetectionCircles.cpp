#include "DetectionCircles.h"


DetectionCircles::DetectionCircles(void) : _distanceBetweenPoints(0.0)
{
}


DetectionCircles::~DetectionCircles(void)
{
}


void DetectionCircles::calculateDistanceBetweenPoints()
{
	// also remove duplicates in this loop!
	// all available distances are saved in this vector from all points to all points
	vector<double> _distancesBetweenAllCircles;
	//check all circles
	if (circles.size()>=5)
	{
		for(int i=0;i<circles.size();i++)
		{
			for(int j=i+1;j<circles.size();j++)
			{
				// distance between points
				_distancesBetweenAllCircles.push_back(distanceOfPoints(circles[i],circles[j]));
			}
		}
		std::sort(_distancesBetweenAllCircles.begin(),_distancesBetweenAllCircles.end());
		// drop smallest value (i=0) and use all that are smaller than the smallest possible distance diagonal
		double distanceSum = 0;
		int count=0;
		for(int i=1;i<_distancesBetweenAllCircles.size();i++)
		{
			if (_distancesBetweenAllCircles[i] < _distancesBetweenAllCircles[1]*sqrt(2)) {distanceSum+=_distancesBetweenAllCircles[i];count++;}
		}
		_distanceBetweenPointsTopResults_temp.push_back(distanceSum/count);
	}
	// if 3 results, then 
	if (_distanceBetweenPointsTopResults_temp.size() == 3)
	{
		_distanceBetweenPoints=(_distanceBetweenPointsTopResults_temp[0]+_distanceBetweenPointsTopResults_temp[1]+_distanceBetweenPointsTopResults_temp[2])/3;
		_distanceBetweenPointsTopResults_temp.clear();
	}
}


void DetectionCircles::detect(Mat* frame, Rect* face)
{
	_frame=frame;
	if (calculateCircleArea(face))
	{
	circles.clear();
	// if at least 4 circles were recognized
	if (detectCircles())
	{
		if (_distanceBetweenPoints == 0.0) {calculateDistanceBetweenPoints();}
		//now use distanceBetweenPoints and focal length to calculate basic distance
		else {fillCircleMatrix();}
	}
	// now I have a basic value for first distance calculation.
	// we need also the rotation of the points in straight view to cam.
	// Now I have to create a matrix, that will be moved with the help of at least 3 detected points (2 cols,1 row or 1row,2cols)
	// with these 3 points we can calculate the other 6 points
	// if same rotation like straight view, distance can be can be calculated by change in distance between the points
	// first aim keep straight view and closer and farer (pixel count change) & drawing matrix
	// eye reg relative to headband
	// stürzt ab bei 3 cams oder mehr
	// min und mx punkte für ecken bestimmen.
	}
}

int DetectionCircles::getNeighborInMatrix(int i,int side)
{
	// 0=left
	if (side == 0 && i%3 != 0){i--;}
	// 1=right
	if (side == 1 && i&3 != 2){i++;}
	//2=above
	if (side == 2 && i> 2){i=i-3;}
	//3=under
	if (side == 3 && i< 7){i=i+3;}
	return i;
}

bool DetectionCircles::calc3rdOutOf2(int a,int b,int c)
{
	bool change=false;
	int findOut;
	if(circleMatrix[a]==Point(0,0) && circleMatrix[b]!=Point(0,0) && circleMatrix[c]!=Point(0,0)) 
	{ 
		circleMatrix[a].x=circleMatrix[b].x-(circleMatrix[c].x-circleMatrix[b].x );
		circleMatrix[a].y=circleMatrix[b].y-(circleMatrix[c].y-circleMatrix[b].y );
		change=true;
	}
	if(circleMatrix[a]!=Point(0,0) && circleMatrix[b]==Point(0,0) && circleMatrix[c]!=Point(0,0)) 
	{
		circleMatrix[b].x=circleMatrix[a].x+0.5*(circleMatrix[c].x-circleMatrix[a].x );
		circleMatrix[b].y=circleMatrix[a].y+0.5*(circleMatrix[c].y-circleMatrix[a].y );
		change=true;
	} 
	if(circleMatrix[a]!=Point(0,0) && circleMatrix[b]!=Point(0,0) && circleMatrix[c]==Point(0,0)) 
	{
		circleMatrix[c].x=circleMatrix[b].x+(circleMatrix[b].x-circleMatrix[a].x );
		circleMatrix[c].y=circleMatrix[b].y+(circleMatrix[b].y-circleMatrix[a].y );
		change=true;
	} 
	return change;
}
void DetectionCircles::fillCircleMatrix()
{
	// getting corner points
	// set first circle in every corner
	// get xMin and yMin
	int xMin=circles[0].x,yMin=circles[0].y,xMax=circles[0].x,yMax=circles[0].y;;
	for (int i=1;i<circles.size();i++)
	{
		if (circles[i].x < xMin) {xMin=circles[i].x;}
		if (circles[i].y < yMin) {yMin=circles[i].y;}
		if (circles[i].x > xMax) {xMax=circles[i].x;}
		if (circles[i].y > yMax) {yMax=circles[i].y;}
	}
	int xMid=xMin+(xMax-xMin)/2;
	int yMid=yMin+(yMax-yMin)/2;
	// empty previous values
	circleMatrix.clear();
	bool row0=false,row1=false,row2=false,col0=false,col1=false,col2=false;
	for (int i=0;i<circles.size();i++)
	{
		if (nearlyEqual(circles[i].x,xMin) && nearlyEqual(circles[i].y,yMin)) {circleMatrix[0]=circles[i];row0=true;col0=true;}
		if (nearlyEqual(circles[i].x,xMid) && nearlyEqual(circles[i].y,yMin)) {circleMatrix[1]=circles[i];row0=true;col1=true;}
		if (nearlyEqual(circles[i].x,xMax) && nearlyEqual(circles[i].y,yMin)) {circleMatrix[2]=circles[i];row0=true;col2=true;}
		if (nearlyEqual(circles[i].x,xMin) && nearlyEqual(circles[i].y,yMid)) {circleMatrix[3]=circles[i];row1=true;col0=true;}
		if (nearlyEqual(circles[i].x,xMid) && nearlyEqual(circles[i].y,yMid)) {circleMatrix[4]=circles[i];row1=true;col1=true;}
		if (nearlyEqual(circles[i].x,xMax) && nearlyEqual(circles[i].y,yMid)) {circleMatrix[5]=circles[i];row1=true;col2=true;}
		if (nearlyEqual(circles[i].x,xMin) && nearlyEqual(circles[i].y,yMax)) {circleMatrix[6]=circles[i];row2=true;col0=true;}
		if (nearlyEqual(circles[i].x,xMid) && nearlyEqual(circles[i].y,yMax)) {circleMatrix[7]=circles[i];row2=true;col1=true;}
		if (nearlyEqual(circles[i].x,xMax) && nearlyEqual(circles[i].y,yMax)) {circleMatrix[8]=circles[i];row2=true;col2=true;}
	}
	// if 2, add third
	
	// each row and column must have at least one value
	if (row0 && row2 && col0 && col1 && col2 || row0 && row1 && row2 && col0 && col2) 
	{
	//while (calc3rdOutOf2(0,1,2) | calc3rdOutOf2(3,4,5) | calc3rdOutOf2(6,7,8) | calc3rdOutOf2(0,3,6) | calc3rdOutOf2(1,4,7) | calc3rdOutOf2(2,5,8));

	}
	else {circleMatrix.clear();}
	
	// calculate missing circles
	// circleMatrix must have at least 3 entries. But they should not be in one row

	// rows
	

}

bool DetectionCircles::calculateCircleArea(Rect* face)
{
	//check if a face was already recognized
	if (face->x)
	{
	circleArea.x=(int)(face->x+0.25*(double)face->width);
	circleArea.y=max((int)(face->y-0.2*(double)face->height),0);
	circleArea.width=(int)(0.5*(double)face->width);
	circleArea.height=(int)(0.5*(double)face->height);
	return true;
	}
	else {return false;}
}


bool DetectionCircles::detectCircles()
{
	cvtColor( (*_frame)(circleArea), _working_frame, CV_BGR2GRAY );
	threshold(_working_frame, _working_frame,  120, 255, CV_THRESH_BINARY );
	Canny(_working_frame, _working_frame, 255, 255, 3);
	vector<vector<Point>> contours;
	vector<Point> approx;
    findContours( _working_frame, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
for (int i = 0; i < contours.size(); i++)
{
    // Approximate contour with accuracy proportional
    // to the contour perimeter
    cv::approxPolyDP(Mat(contours[i]),approx,arcLength(Mat(contours[i]), true) * 0.02,true);

	if (approx.size() > 6)
	{
        // Detect and label circles
        double area = contourArea(contours[i]);
        cv::Rect r = boundingRect(contours[i]);
        int radius = r.width / 2;
        if (abs(1 - ((double)r.width / r.height)) <= 0.2 & abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.2)
        {
			// check if circle already in vector
			int x = r.x+r.width/2;
			int y = r.y+r.height/2;
			// add first circle. Also say that circle 0 is in row 2
			if (circles.empty()) 
			{
				circles.push_back(Point(x,y));
			}
			// add all unique circles
			bool exists=false;
			for(int i=0;i<circles.size();i++)
			{
				if (nearlyEqual(circles[i].x,x) && nearlyEqual(circles[i].y,y)) {exists=true;}
			}
			if (!exists) 
			{
				//add to circles the new one
				circles.push_back(Point(x,y));
			}
        }
    }
}

if (circles.size()<4)
{
	return false;
}
else {return true;}
}

 Rect* DetectionCircles::getCircleArea()
 {
	 return &circleArea;
 }


bool DetectionCircles::nearlyEqual(int x,int y)
{
	if (x <= 1.05*y && x >= 0.95*y) {return true;}
	else {return false;}
}

double DetectionCircles::distanceOfPoints(Point i,Point j)
{
	return sqrt(pow((i.x-j.x),2)+pow((i.y-j.y),2));
}