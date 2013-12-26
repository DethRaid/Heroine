#include <ioostream>
#include <fstream>
#include <vector>

#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

#define NUM_POINTS 150;
#define QUALITY_LEVEL 1.0;
#define MIN_DISTANCE 20.0;
#define DESIRED_QUALITY 0.9;

int main();

int main() {
	fstream uart1( "/dev/ttyO1" );
	if( !fstream.open() ) {
		cout << "ERROR: Could not open serial stream. No communication will be available\n";
	} else {
		cout << "Serial stream opened successfully\n";
	}

	//One for the right camera, one for the left
	VideoCapture cam1( 0 );
	if( !cam1.isOpened() ) {
		cout << "ERROR: Could not open camera 1. Exiting";
		return -1;
	}
	VideoCapture cam2( 1 );
	if( !cam2.isOpened( ) ) {
		cout << "ERROR: Could not open camera 2. Exiting";
		return -1;
	}
	cout << "Cameras opened suffessfully\n";

	Mat rFrame1, rFrame2, lFrame1, lFrame2, depthImg;
	vector<Point2f> points1, points2, goodPoints1, goodPoints2;
	vector<unsigned char> status;
	vector<float> err;

	pointsToTrack1.reserve( NUM_POINTS );
	pointsToTrack2.reserve( NUM_POINTS );
	status.reserve( NUM_POINTS );
	err.reserve( NUM_POINTS );

	StereoBM *stereoCompute = createStereoBM( 0, 13 );

	namedWindow( "Camera1", 1 );
	namedWindow( "Camera2", 2 );
	namedWindow( "Output", 3 );

	cam1 >> rFrame1;
	cvtColor( rFrame1, rFrame1, COLOR_BGR2GRAY );
	goodFeaturesToTrack( rFrame1, pointsToTrack, NUM_POINTS, QUALITY_LEVEL, MIN_DISTANCE );
	
	while( true ) {
		//swap frames one and two, for purposes of object tracking
		rFrame2 = rFrame1.copy();
		lFrame2 = lFrame1.copy();
		cam1 >> rFrame1;
		cam2 >> lFrame1;

		cvtColor( rFrame1, rFrame1, COLOR_BGR2GRAY );
		cvtColor( lFrame1, lFrame1, COLOR_BGR2GRAY );

		calcOpticalFlowPyrLK( rFrame2, rFrame1, points2, points1, status, err );

		goodPoints1.clear();
		goodPoints2.clear();

		for( int i = 0; i < status.size(); i++ ) {
			if( status[i] > DESIRED_QUALITY ) {
				goodPoints1.pushBack( points1[i] );
				goodPoints2.pushBack( points2[i] );
			}
		}

		//so now we have a list of all the good points and their positions on this frame and the last
		//We should keep their movement vectors so we can calculate the movement of a given object


		
		stereoCompute.compute( lFrame1, rFrame1, depthImg );
		//depthImg is now the depth image, with a depth of 1 being no distance and a depth of 0 being all the depth

		imshow( "Output", depthImg );

		if( waitKey( 30 ) >= 0 ) {
			break;
		}
	}
	return 0;
}