#include "DetectionPupil.h"


DetectionPupil::DetectionPupil(void) : _helper(0,0)
{
}


DetectionPupil::~DetectionPupil(void)
{
}


Point DetectionPupil::calcHelper(int leftRight)
{
	Rect rectForHelper(_working_frame.cols*0.15,_working_frame.rows*0.15,_working_frame.cols*0.7,_working_frame.rows*0.7);
	vector<Point> pointsForHelper;
	Mat test =_working_frame.clone();
	test(rectForHelper);
// -----------
		int darkest=0;
	Point darkestPoint;
    for (int i = 0; i < test.cols; i++ ) {
        for (int j = 0; j < test.rows; j++) {
            if (test.at<uchar>(j, i) > darkest) {   
                darkest=test.at<uchar>(j, i);
				darkestPoint=Point(i,j);
            }
        }
	}
	int lightest=255;
	Point lightestPoint;
    for (int i = 0; i < test.cols; i++ ) {
        for (int j = 0; j < test.rows; j++) {
            if (test.at<uchar>(j, i) < lightest) {   
                lightest=test.at<uchar>(j, i);
				lightestPoint=Point(i,j);
            }
        }
	}
	int middle=(darkest-lightest)/2+lightest; // bei 100 bis 200 --> 150
//		if (leftRight==0){imshow("test",test);}
    for (int i = 0; i < test.cols; i++ ) {
        for (int j = 0; j < test.rows; j++) {
			if (test.at<uchar>(j, i) >= middle) {test.at<uchar>(j, i)=255;} 
			else {
				test.at<uchar>(j, i)=(int)((float)(test.at<uchar>(j, i)-lightest)/(float)(middle-lightest)*255);}
        }
	}
	//equalizeHist( test,test);
	threshold(test,test,0,255,THRESH_BINARY_INV);

// ------------
	//threshold(test, test,  0, 220, CV_THRESH_BINARY );
	//bitwise_not( test, test );
	Canny(test, test, 0, 240, 5);
	
	//if (leftRight==0){imshow("test2",test);}
	vector<vector<Point>> contours;
	vector<Point> approx,circles;


	findContours( test, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
	Point pupilCenter(0,0);
	int radius=0;
	for (int i = 0; i < contours.size(); i++)
	{
		// Approximate contour with accuracy proportional
		// to the contour perimeter
		cv::approxPolyDP(Mat(contours[i]),approx,arcLength(Mat(contours[i]), true) * 0.02,true);
		if (approx.size() > 3)
			{
		        // Detect and label circles
		        double area = contourArea(contours[i]);
		        cv::Rect r = boundingRect(contours[i]);
		        if ((r.width/2)>radius) 
				{
				radius = r.width / 2;
				// check if circle already in vector
				pupilCenter=Point(r.x+r.width/2,r.y+r.height/2);
				//int x = r.x+r.width/2;
				//int y = r.y+r.height/2;
				//circle(test,Point(x,y),radius,Scalar(255,255,0));
				}
			}
	}
	return pupilCenter;
}
	

	//for (int i = 0; i < contours.size(); i++)
	//{
 //       double area = contourArea(contours[i]);
 //       cv::Rect r = boundingRect(contours[i]);
	//	pointsForHelper.push_back(Point(r.x+rectForHelper.x,r.y+=rectForHelper.y));
	//}
	//if(pointsForHelper.size()>0)
	//{
	//int x=0,y=0,x2=0,y2=0,count=0;
	//for (int p=0;p<pointsForHelper.size();p++)
	//{
	//	x+=pointsForHelper[p].x;y+=pointsForHelper[p].y;
	//}
	//x/=pointsForHelper.size();y/=pointsForHelper.size();
	//for (int p=0;p<pointsForHelper.size();p++)
	//{
	//	if (nearlyEqual(pointsForHelper[p].x,x,40) && nearlyEqual(pointsForHelper[p].y,y,40))
	//	{
	//		x2+=pointsForHelper[p].x;y2+=pointsForHelper[p].y;
	//		count++;
	//	}
	//}
	//pointsForHelper.clear();
	//if (count>0) {_helper=Point(x2/count+_eyeRect.x,y2/count+_eyeRect.y);return true;}
	//}



Point DetectionPupil::findEyeCenter(Mat* frame, Rect eyeRect,int leftRight) {
	_eyeRect=eyeRect;
	//_working_frame=(*frame)(_eyeRect);
	//vector<Mat> bgr_planes;
	 //split( _working_frame, bgr_planes );
	 //if (leftRight==0){imshow("test",bgr_planes[2]);}
	 //if (leftRight==0){imshow("test2", bgr_planes[1]);}
	cvtColor((*frame)(_eyeRect), _working_frame, CV_BGR2GRAY );
	//equalizeHist( _working_frame, _working_frame );
	Point point1=calcHelper(leftRight);
	point1.x+=_eyeRect.x;point1.y+=_eyeRect.y;
	return point1;
	//if (calcHelper(leftRight) != Point(0,0)) 
	//{
	//	_eyeRect.x=_helper.x-0.3*eyeRect.width;
	//	_eyeRect.y=_helper.y-0.3*eyeRect.height;
	//	_eyeRect.width=0.4*_eyeRect.width;
	//	_eyeRect.height=0.6*_eyeRect.height;
	//	_working_frame=(*frame)(_eyeRect);
	//	cvtColor((*frame)(_eyeRect), _working_frame, CV_BGR2GRAY );
	//	equalizeHist( _working_frame, _working_frame );
	//}
	//Mat test,test2;
	//cvtColor((*frame)(_eyeRect), test, CV_RGB2GRAY );
	//cvtColor((*frame)(_eyeRect), test2, CV_RGB2GRAY );
	//// find darkest point
	//bitwise_not(test2,test2);
	//int darkest=0;
	//Point darkestPoint;
 //   for (int i = 0; i < test2.cols; i++ ) {
 //       for (int j = 0; j < test2.rows; j++) {
 //           if (test2.at<uchar>(j, i) > darkest) {   
 //               darkest=test2.at<uchar>(j, i);
	//			darkestPoint=Point(i,j);
 //           }
 //       }
	//}
	//int lightest=255;
	//Point lightestPoint;
 //   for (int i = 0; i < test2.cols; i++ ) {
 //       for (int j = 0; j < test2.rows; j++) {
 //           if (test2.at<uchar>(j, i) < lightest) {   
 //               lightest=test2.at<uchar>(j, i);
	//			lightestPoint=Point(i,j);
 //           }
 //       }
	//}
	//threshold(test2,test2,lightest+20,darkest-20,THRESH_BINARY);
	//equalizeHist( test2,test2);
	//morphologyEx(test2,test2,4,cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3)));
	//Mat test3=test2.clone();
	//Canny(test2, test2, 0, 255, 3);
	//morphologyEx(test2,test2,4,cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3)));
	////bitwise_not(test2,test2);
	//
	//for (int i = 0; i < test.cols; i++ ) {
 //       for (int j = 0; j < test.rows; j++) {
	//		if (test2.at<uchar>(j, i) >128)
	//		{test.at<uchar>(j, i)=(test.at<uchar>(j, i)+test2.at<uchar>(j, i))/2;}
 //       }
	//}
	//_working_frame=test.clone();
	//morphologyEx(test2,test2,4,cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3)));
	//Canny(test2, test2, 0, 255, 3);
    /*for (int i = 0; i < test2.cols; i++ ) {
        for (int j = 0; j < test2.rows; j++) {
            if (test2.at<uchar>(j, i) == darkest) {   
                circle(test2,Point(i,j),3,Scalar(255,0,0));
            }
        }
	}*/


	//threshold(test2,test2,220,255,THRESH_BINARY);
	//threshold(test2, test, 210, 255, CV_THRESH_BINARY);
	//morphologyEx(test2,test2,4,cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3)));
	//equalizeHist( test2,test2);
	//adaptiveThreshold(test2,test2,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,11,3);
	//adaptiveThreshold(test2,test2,255,ADAPTIVE_THRESH_GAUSSIAN_C,\
 //           THRESH_BINARY,11,5);
	////bitwise_not(test2,test2);
	////if (leftRight==0){imshow("test",test2);}
	//medianBlur(test2,test2,5);
	//bitwise_not(test2,test2);
	//Canny(test2, test2, 0, 255, 3);
	//if (leftRight==0){imshow("test2",test2);}
	////if (leftRight==0){imshow("test2",test2);}
	//vector<vector<Point>> contours;
	//vector<Point> approx;
 //   findContours( test2, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
	//for (int i = 0; i < contours.size(); i++)
	//{
 //   // Approximate contour with accuracy proportional
 //   // to the contour perimeter
 //   cv::approxPolyDP(Mat(contours[i]),approx,arcLength(Mat(contours[i]), true) * 0.02,true);

	//if (approx.size() > 20)
	//{
 //       // Detect and label circles
 //       double area = contourArea(contours[i]);
 //       cv::Rect r = boundingRect(contours[i]);
 //       int radius = r.width / 2;
	//	// check if circle already in vector
	//		int x = r.x+r.width/2;
	//		int y = r.y+r.height/2;
	//		circle(test,Point(x,y),radius,Scalar(255,255,0));
	//}
	//}
	//if (leftRight==0){imshow("test",test);}
	
	

	
	//vector<Mat> bgr_planes;
	//split( test, bgr_planes );
	//bitwise_not(test,test2);
	//if (leftRight==0){imshow("test",test);}
	//equalizeHist( bgr_planes[2], test2 );
	//threshold(test, test2,  100, 255, 3 );
	//Canny(test, test2, 150, 255, 3);
	//if (leftRight==0){imshow("test2", test2);}
	//
	//Canny(test, test2, 255, 255, 3);
	//vector<vector<Point>> contours;
	//vector<Point> approx,circles;
 //   findContours( test2, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
	//for (int i = 0; i < contours.size(); i++)
	//{
 //       double area = contourArea(contours[i]);
 //       cv::Rect r = boundingRect(contours[i]);
	//	rectangle(test,r,Scalar(255,0,0));
	//}
	//

	scaleToFastSize(_working_frame, _working_frame);
	//-- Find the gradient
	Mat gradientX = computeMatXGradient(_working_frame);
	Mat gradientY = computeMatXGradient(_working_frame.t()).t();
	//-- Normalize and threshold the gradient
	// compute all the magnitudes
	cv::Mat mags = matrixMagnitude(gradientX, gradientY);
	//compute the threshold
	double gradientThresh = computeDynamicThreshold(mags, 50.0);
	//double gradientThresh = kGradientThreshold;
	//double gradientThresh = 0;
	//normalize
	for (int y = 0; y < _working_frame.rows; ++y) {
		double *Xr = gradientX.ptr<double>(y), *Yr = gradientY.ptr<double>(y);
		const double *Mr = mags.ptr<double>(y);
		for (int x = 0; x < _working_frame.cols; ++x) {
			double gX = Xr[x], gY = Yr[x];
			double magnitude = Mr[x];
			if (magnitude > gradientThresh) {
				Xr[x] = gX/magnitude;
				Yr[x] = gY/magnitude;	
			} else {
				Xr[x] = 0.0;
				Yr[x] = 0.0;
			}
		}
	}
	
	//imshow(debugWindow,gradientX);
	//-- Create a blurred and inverted image for weighting
	Mat weight;
	GaussianBlur( _working_frame, weight, Size( 5, 5 ), 0, 0 );
	for (int y = 0; y < weight.rows; ++y) {
		unsigned char *row = weight.ptr<unsigned char>(y);
		for (int x = 0; x < weight.cols; ++x) {
			row[x] = (255 - row[x]);
		}
	}
	//-- Run the algorithm!
	Mat outSum = cv::Mat::zeros(_working_frame.rows,_working_frame.cols,CV_64F);
	// for each possible center
	// printf("Eye Size: %ix%i\n",outSum.cols,outSum.rows);
	for (int y = 0; y < weight.rows; ++y) {
		const unsigned char *Wr = weight.ptr<unsigned char>(y);
		const double *Xr = gradientX.ptr<double>(y), *Yr = gradientY.ptr<double>(y);
		for (int x = 0; x < weight.cols; ++x) {
			double gX = Xr[x], gY = Yr[x];
			if (gX == 0.0 && gY == 0.0) {
				continue;
			}
			testPossibleCentersFormula(x, y, Wr[x], gX, gY, outSum);
		}
	}
	// scale all the values down, basically averaging them
	double numGradients = (weight.rows*weight.cols);
	Mat out;
	outSum.convertTo(out, CV_32F,1.0/numGradients);
	//imshow(debugWindow,out);
	//-- Find the maximum point
	cv::Point maxP;
	double maxVal;
	cv::minMaxLoc(out, NULL,&maxVal,NULL,&maxP);
	//-- Flood fill the edges
	if(true) {
		cv::Mat floodClone;
		//double floodThresh = computeDynamicThreshold(out, 1.5);
		double floodThresh = maxVal * 0.97f;
		cv::threshold(out, floodClone, floodThresh, 0.0f, cv::THRESH_TOZERO);
		cv::Mat mask = floodKillEdges(floodClone);
		//imshow(debugWindow + " Mask",mask);
		//imshow(debugWindow,out);
		// redo max
		cv::minMaxLoc(out, NULL,&maxVal,NULL,&maxP,mask);
	}
	maxP=unscalePoint(maxP,Point(_eyeRect.width,_eyeRect.height));
	Mat test=(*frame)(_eyeRect);
	circle(test,maxP,3,Scalar(255,0,255));
	maxP.x+=_eyeRect.x;maxP.y+=_eyeRect.y;
	circle(test,point1,3,Scalar(255,255,0));
	if (leftRight==0){imshow("test",test);}
	return maxP;
}

//resizing the Eye region by dividing eye width with rows, cols
void DetectionPupil::scaleToFastSize(const cv::Mat &src,cv::Mat &dst) {
	cv::resize(src, dst, cv::Size(50,(((float)50)/src.cols) * src.rows));
}
Point DetectionPupil::unscalePoint(Point p, Point origSize) {
	float ratio = (((float)50.0)/origSize.x);
	int x = (p.x / ratio);
	int y = (p.y / ratio);
	return Point(x,y);
}

bool DetectionPupil::inMat(Point p,int rows,int cols) {
	return p.x >= 0 && p.x < cols && p.y >= 0 && p.y < rows;
}


bool DetectionPupil::floodShouldPushPoint(const cv::Point &np, const cv::Mat &mat) {
	return inMat(np, mat.rows, mat.cols);
}


Mat DetectionPupil::floodKillEdges(cv::Mat &mat) {
	rectangle(mat,cv::Rect(0,0,mat.cols,mat.rows),255);
	//refines undefined edges my masking
	cv::Mat mask(mat.rows, mat.cols, CV_8U, 255);
	std::queue<Point> toDo;
	toDo.push(cv::Point(0,0));
	while (!toDo.empty()) {
		cv::Point p = toDo.front();
		toDo.pop();
		if (mat.at<float>(p) == 0.0f) {
			continue;
		}
		// add in every direction
		cv::Point np(p.x + 1, p.y); // right
		if (floodShouldPushPoint(np, mat)) toDo.push(np);
		np.x = p.x - 1; np.y = p.y; // left
		if (floodShouldPushPoint(np, mat)) toDo.push(np);
		np.x = p.x; np.y = p.y + 1; // down
		if (floodShouldPushPoint(np, mat)) toDo.push(np);
		np.x = p.x; np.y = p.y - 1; // up
		if (floodShouldPushPoint(np, mat)) toDo.push(np);
		// kill it
		mat.at<float>(p) = 0.0f;
		mask.at<uchar>(p) = 0;
	}
	return mask;
}

void DetectionPupil::testPossibleCentersFormula(int x, int y, unsigned char weight,double gx, double gy, Mat &out) {
	// for all possible centers 
	for (int cy = 0; cy < out.rows; ++cy) {
		double *Or = out.ptr<double>(cy);
		for (int cx = 0; cx < out.cols; ++cx) {
			if (x == cx && y == cy) {
				continue;
			}
			// create a vector, so called displacement vector, from the possible center to the gradient origin
			double dx = x - cx;
			double dy = y - cy;
			// normalize displacement vector

			double magnitude = sqrt((dx * dx) + (dy * dy));
			dx = dx / magnitude;
			dy = dy / magnitude;
			//find dot product between gradient and displacement vector to exploit the orientation between g and d
			double dotProduct = dx*gx + dy*gy;
			dotProduct = max(0.0,dotProduct);
			// square and multiply by the weight to derive the dark centres than other centre
			if (false) {
				Or[cx] += dotProduct * dotProduct * (weight/150.0);
			} else {
				Or[cx] += dotProduct * dotProduct;
			}
		}
	}
}


Mat DetectionPupil::computeMatXGradient(const Mat &mat) {
	Mat out(mat.rows,mat.cols,CV_64F);

	for (int y = 0; y < mat.rows; ++y) {
		const uchar *Mr = mat.ptr<uchar>(y);
		double *Or = out.ptr<double>(y);

		Or[0] = Mr[1] - Mr[0];
		for (int x = 1; x < mat.cols - 1; ++x) {
			Or[x] = (Mr[x+1] - Mr[x-1])/2.0;
		}
		Or[mat.cols-1] = Mr[mat.cols-1] - Mr[mat.cols-2];
	}
	return out;
}


Mat DetectionPupil::matrixMagnitude(const cv::Mat &matX, const cv::Mat &matY) {
	cv::Mat mags(matX.rows,matX.cols,CV_64F);
	for (int y = 0; y < matX.rows; ++y) {
		const double *Xr = matX.ptr<double>(y), *Yr = matY.ptr<double>(y);
		double *Mr = mags.ptr<double>(y);
		for (int x = 0; x < matX.cols; ++x) {
			double gX = Xr[x], gY = Yr[x];
			double magnitude = sqrt((gX * gX) + (gY * gY));
			Mr[x] = magnitude;
		}
	}
	return mags;
}


bool DetectionPupil::nearlyEqual(int x,int y,int z)
{
	if (x <= (1.00+(((double)z)/100))*y && x >= (1.00-(((double)z)/100))*y) {return true;}
	else {return false;}
}


double DetectionPupil::computeDynamicThreshold(const cv::Mat &mat, double stdDevFactor) {
	cv::Scalar stdMagnGrad, meanMagnGrad;
	cv::meanStdDev(mat, meanMagnGrad, stdMagnGrad);
	double stdDev = stdMagnGrad[0] / sqrt(mat.rows*mat.cols);
	return stdDevFactor * stdDev + meanMagnGrad[0];
}

