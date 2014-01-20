#include <ctime>

#include <iostream>
#include <forward_list>
#include <fstream>
#include <vector>

#include <opencv2/opencv.hpp>

#include "ObjectPoint3f.h"
//********//********//********//********//********//********//********//********

//Coded with OpenCV 3.0.0

using namespace std;
using namespace cv;

#define NUM_POINTS 150
#define QUALITY_LEVEL 0.01
#define MIN_DISTANCE 20.0
#define DESIRED_QUALITY 0.5

int main();
void findCorrespondingPoints( Mat imgR, Mat imgL, vector<Point2f> ptsR, 
    vector<Point2f> ptsL );
int errorExit( string msg );
Point2f normalize( Point2f &p );
float magnitude( Point2f &p );
void loadCameraMatrices();

//The camera matrixes for distortion and whatnot
Mat lCamMat, rCamMat, rDistCoeff, lDistCoeff;
Mat rFrameOld, rFrameNew, lFrameOld, lFrameNew;
vector<int> depths;
vector<Point2f> rPointsOld, rPointsNew, lPoints;
vector<Point2f> goodPointsOld, goodPointsNew, directions;
vector<ObjectPoint3f> objectPoints;
vector<Point3f> worldPoints;
vector<vector<int>> objects;
vector<unsigned char> status;
int colors[255];
vector<float> err;
float objDepthFuzz = 30.0f;	//sample value. Will need tuning.
time_t startTime;

int errorExit( string msg ) {
    cout << msg;
    cin.get();
    return -1;
}

int main() {
    fstream uart1( "/dev/ttyO1" );
    if( !uart1.is_open() ) {
        cout << "ERROR: Could not open serial stream. No communication will be"
            << " available\n";
    } else {
        cout << "Serial stream opened successfully\n";
    }

    //One for the right camera, one for the left
    VideoCapture cam1( 0 );
    if( !cam1.isOpened() ) {
        return errorExit( "ERROR: Could not open camera 1. Exiting\n" );
    }
    VideoCapture cam2( 1 );
    if( !cam2.isOpened() ) {
        return errorExit( "ERROR: Could not open camera 2. Exiting" );
    }
    cout << "Cameras opened suffessfully\n";

    srand( time( NULL ) );

    for( int i = 0; i < 255; i++ ) {
        colors[i] = rand() % 255;
    }

    rPointsOld.reserve( NUM_POINTS );
    rPointsNew.reserve( NUM_POINTS );
    status.reserve( NUM_POINTS );
    err.reserve( NUM_POINTS );

    namedWindow( "Camera1", 1 );
    namedWindow( "Camera2", 2 );
    namedWindow( "Output", 3 );

    cam1 >> rFrameNew;
    cvtColor( rFrameNew, rFrameNew, COLOR_BGR2GRAY );

    cam2 >> lFrameNew;
    cvtColor( lFrameNew, lFrameNew, COLOR_BGR2GRAY );
    //undistort( rFrameOld, rFrameOld, rCamMat, rDistCoeff );
    //This function creates no previous points. Will fix after dinner.
    //Not sure which dinner
    goodFeaturesToTrack( rFrameNew, rPointsOld, NUM_POINTS, QUALITY_LEVEL,
        MIN_DISTANCE );

    //Find the corresponding points in the other frame
    calcOpticalFlowPyrLK( rFrameNew, lFrameNew, rPointsOld, lPoints,
        status, err );

    if( rPointsOld.size() != lPoints.size() ) {
        cout << "Error: Could not find all requested points. Attempting to "
            << "continue anyway. Expect a bad time\n";
    }


    for( int i = 0; i < lPoints.size(); i++ ) {
        circle( lFrameNew, lPoints[i], 10, colors[i] );
        circle( rFrameNew, rPointsOld[i], 10, colors[i] );
        //disparity = x - x' = Bf/Z
        //Z(x - x') = Bf
        //Z = Bf/(x - x')
        //B = 6.35 cm (about distance between human eyes)
        //   B = distance between cameras
        //f = 700 (units of some sort? OpenCV claims this is in pixels)
        //  Possibly this is in sensor pixels? Odd unit
        //  f = focal length of camera (it should be the same for both)
        //Bf = 4445
        //  It's just math
        depths.push_back( 4445 / (lPoints[i].x - rPointsOld[i].x) );

        //We now have the points we can track on both images. Time to whip out 
        //our robes and wizard hats!

        //Divide everything by z so that a point farther away will be closer to
        //the origin for a given (u, v)
        worldPoints.push_back( Point3f( lPoints[i].x / depths[i],
            lPoints[i].y / depths[i],
            depths[i] ) );
    }

    imshow( "Camera1", rFrameNew );
    imshow( "Camera2", lFrameNew );

    waitKey( 30 );

    while( true ) {
        startTime = time( NULL );
        cout << "Starting calculations at " << startTime << "\n";
        //swap frames one and two, for purposes of object tracking
        rFrameOld = rFrameNew.clone();
        lFrameOld = lFrameNew.clone();
        cam1 >> rFrameNew;
        cam2 >> lFrameNew;

        cvtColor( rFrameNew, rFrameNew, COLOR_BGR2GRAY );
        cvtColor( lFrameNew, lFrameNew, COLOR_BGR2GRAY );

        /* Find corresponding points on the left image */
        calcOpticalFlowPyrLK( rFrameNew, lFrameNew, rPointsOld, lPoints,
            status, err );

        for( int i = 0; i < lPoints.size(); i++ ) {
            circle( lFrameNew, lPoints[i], 10, colors[i] );
            circle( rFrameNew, rPointsOld[i], 10, colors[i] );
            //disparity = x - x' = Bf/Z
            //Z(x - x') = Bf
            //Z = Bf/(x - x')
            //B = 6.35 cm (about distance between human eyes)
            //   B = distance between cameras
            //f = 700 (units of some sort? OpenCV claims this is in pixels)
            //  Possibly this is in sensor pixels? Odd unit
            //  f = focal length of camera (it should be the same for both)
            //Bf = 4445
            //  It's just math
            depths.push_back( 4445 / (lPoints[i].x - rPointsOld[i].x) );

            //We now have the points we can track on both images. Time to whip out 
            //our robes and wizard hats!

            //Divide everything by z so that a point farther away will be closer to
            //the origin for a given (u, v)
            worldPoints.push_back( Point3f( lPoints[i].x / depths[i],
                lPoints[i].y / depths[i],
                depths[i] ) );
        }

        imshow( "Camera1", rFrameNew );
        imshow( "Camera2", lFrameNew );

        cout << "Found corresponding stereo points at " << time( NULL ) << ", "
            << time( NULL ) - startTime << " seconds later\n";

        /* Find corresponding points on the next image */

        calcOpticalFlowPyrLK( rFrameOld, rFrameNew, rPointsOld, rPointsNew,
            status, err );

        goodPointsOld = goodPointsNew;
        goodPointsNew.clear();

        objectPoints.clear();
        directions.clear();
        for( int i = 0; i < status.size(); i++ ) {
            //if( status[i] > DESIRED_QUALITY ) {
                goodPointsOld.push_back( rPointsOld[i] );
                goodPointsNew.push_back( rPointsNew[i] );
                objectPoints.push_back( rPointsNew[i] );
                //so now we have a list of all the good points and their
                //positions on this frame and the last
                //We should keep their movement vectors so we can calculate the
                //movement of a given object
                directions.push_back( goodPointsNew[goodPointsNew.size() - 1]
                    - goodPointsOld[goodPointsOld.size() - 1] );
            //}
        }

        rPointsOld = rPointsNew;

        cout << "Found corresponding tracking points at " << time( NULL )
            << ", " << time( NULL ) - startTime << " seconds later\n";

        // stereoCompute->compute( lFrameOld, rFrameOld, depthImg );
        //depthImg is now the depth image, with a depth of 1 being no distance
        //and a depth of 0 being all the depth

        //We have the depth, we have the points we care about. We now need to
        //find which points go with which object
        //Enter recognition of an arbitrary object!

        //if point j is within objDepthFuzz depth value of point i, then point 
        //j is in the same object as point i. Each point needs to know which 
        //object it's in. Of course, this point concatenation only needs to 
        //happen when finding the points for the first time.
        //So, like, every ten frames maybe?

        int curObj = 0;

        ObjectPoint3f *pointI, *pointJ;
        vector<ObjectPoint3f>::iterator begin = objectPoints.begin();
        for( int i = 0; i < objectPoints.size(); i++ ) {
            pointI = &objectPoints[i];
            //has this point been added to any objects yet?
            if( pointI->obj == -1 ) {
                //if not, create a new object with this point in it
                objects.push_back( vector<int>() );
                objects[curObj].push_back( i );
                pointI->obj = curObj;
                curObj++;
            }
            //if so, we can see if anything else needs to be added to this
            //object
            for( int j = i + 1; j < objectPoints.size(); j++ ) {
                pointJ = &objectPoints[j];
                float depthDiff = depths[i] - depths[j];
                if( abs( depthDiff ) < objDepthFuzz ) {
                    pointJ->obj = pointI->obj;
                    //if this throws an error, the algorithm is wrong
                    objects[pointI->obj].push_back( j );
                }
            }
        }



        cout << "Linked up objects at " << time( NULL ) << ", "
             << time( NULL )- startTime << " seconds later\n";

        //at this point we know which direction vectors belong where
        //We can now go about correcting for the camera's rotation, finding the
        //average of each object's vectors, which can be assumed to be the
        //movement of that object. Not sure how we're going to figure out which
        //object is the background (not sure if we need to). We can take each
        //object's movement, project it to next frame, and then go where the
        //object won't be.

        //The number of radians the camera rotated last frame. We need to
        //measure this somehow
        float theta = 0.5;

        //for each point:
        //  subtract p.normalized * sin( theta )
        for( Point2f &p : directions ) {
            Point2f norm = normalize( p );
            float mag = sin( theta );
            norm.x *= mag;
            norm.y *= mag;
            //Not so normalized now, IS IT??!?!?!??!??!?
            p.x -= norm.x;
            p.y -= norm.y;
        }

        cout << "Normalized Directions at " << time( NULL ) << ", " 
             << time( NULL ) - startTime << " seconds later\n";

        //So all the directions should now be corrected for rotation. We can
        //now calculate the average movement of each object

        Point2f *objdirs = new Point2f[objects.size()];
        Point2f avg( 0, 0 );
        float avgDepth = 0;
        for( int i = 0; i < objects.size(); i++ ) {
            for( int j : objects[i] ) {
                avg.x += directions[j].x;
                avg.y += directions[j].y;
                avgDepth += depths[i];
            }
            objdirs[i].x = avg.x / objects[i].size();
            objdirs[i].y = avg.y / objects[i].size();
            avg.x = avg.y = 0;
        }


        cout << "Found average object directsions at " << time( NULL ) << ", " 
             << time( NULL ) - startTime << " seconds later\n";

        //predict where each track will be next frame

        //later

        line( rFrameNew, objectPoints[objects[0][0]],
            objectPoints[objects[0][1]], Scalar( 0, 0, 1 ) );

       /* //draw lines connecting the tracks in a given object
        for( int i = 0; i < objects.size(); i++ ) {
            for( int j = 0; j < objects[i].size() - 1; j++ ) {
                if( j < objects[i].size() ) {
                    line( rFrameNew, objectPoints[objects[i][j]],
                        objectPoints[objects[i][j + 1]], Scalar( 0, 0, 1 ) );
                }
            }
        }


        cout << "Drew Lines at " << time( NULL ) << ", " 
             << time( NULL ) - startTime << " seconds later\n";*/

        if( waitKey( 30 ) >= 0 ) {
            break;
        }
    }

    //write out all points
    ofstream file( "PointCloud.dat" );
    for( int i = 0; i < worldPoints.size(); i++ ) {
        file << worldPoints[i].x << " " << worldPoints[i].y << " "
            << worldPoints[i].z << "\n";
    }

    return 0;
}

Point2f normalize( Point2f &p ) {
    float mag = magnitude( p );
    return Point2f( p.x / mag, p.y / mag );
}

float magnitude( Point2f &p ) {
    return sqrtf( (p.x * p.x) + (p.y * p.y) );
}

void findCorrespondingPoints( Mat imgR, Mat imgL, vector<Point2f> ptsR,
    vector<Point2f> ptsL ) {

}