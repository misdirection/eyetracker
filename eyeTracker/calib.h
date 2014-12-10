#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace std;
using namespace cv;


class Settings
{
public:
	Settings();
	~Settings();
	enum Pattern { NOT_EXISTING, CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };
	enum InputType {INVALID, CAMERA, VIDEO_FILE, IMAGE_LIST};

	void write(cv::FileStorage& fs) const;
	void read(const cv::FileNode& node) ;
	void interprate();
	cv::Mat nextImage();
	static bool readStringList( const string& filename, vector<string>& l );

public:
	Size boardSize;            // The size of the board -> Number of items by width and height
	Pattern calibrationPattern;// One of the Chessboard, circles, or asymmetric circle pattern
	float squareSize;          // The size of a square in your defined unit (point, millimeter,etc).
	int nrFrames;              // The number of frames to use from the input for calibration
	float aspectRatio;         // The aspect ratio
	int delay;                 // In case of a video input
	bool bwritePoints;         //  Write detected feature points
	bool bwriteExtrinsics;     // Write extrinsic parameters
	bool calibZeroTangentDist; // Assume zero tangential distortion
	bool calibFixPrincipalPoint;// Fix the principal point at the center
	bool flipVertical;          // Flip the captured images around the horizontal axis
	string outputFileName;      // The name of the file where to write
	bool showUndistorsed;       // Show undistorted images after calibration
	string input;               // The input ->


	int cameraID;
	vector<string> imageList;
	int atImageList;
	VideoCapture inputCapture;
	InputType inputType;
	bool goodInput;
	int flag;
	VideoCapture* captureDevice;

private:
	string patternToUse;


};

//global functions
static void read(const FileNode& node, Settings& x, const Settings& default_value = Settings())
{
	if(node.empty())
		x = default_value;
	else
		x.read(node);
}
enum { DETECTION = 0, CAPTURING = 1, CALIBRATED = 2 };
static double computeReprojectionErrors( const vector<vector<Point3f> >& objectPoints,
										const vector<vector<Point2f> >& imagePoints,
										const vector<Mat>& rvecs, const vector<Mat>& tvecs,
										const Mat& cameraMatrix , const Mat& distCoeffs,
										vector<float>& perViewErrors);

static void calcBoardCornerPositions(Size boardSize, float squareSize, vector<Point3f>& corners,
									 Settings::Pattern patternType /*= Settings::CHESSBOARD*/);


static bool runCalibration( Settings& s, Size& imageSize, Mat& cameraMatrix, Mat& distCoeffs,
						   vector<vector<Point2f> > imagePoints, vector<Mat>& rvecs, vector<Mat>& tvecs,
						   vector<float>& reprojErrs,  double& totalAvgErr);

bool runCalibrationAndSave(Settings& s, Size imageSize, Mat&  cameraMatrix, Mat& distCoeffs,vector<vector<Point2f> > imagePoints,int );
//main run function

int startCalibration(int m);

void dist_calc(double focal);