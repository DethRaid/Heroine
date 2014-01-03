#include <iostream>
#include <fstream>
#include <set>
#include <vector>

#include <opencv2/opencv.hpp>

#include "ObjectPoint2f.h"
//********//********//********//********//********//********//********//********
using namespace std;
using namespace cv;

#define NUM_POINTS 150
#define QUALITY_LEVEL 1.0
#define MIN_DISTANCE 20.0
#define DESIRED_QUALITY 0.9

int main();
int errorExit( string msg );

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
    if( !cam2.isOpened( ) ) {
        return errorExit( "ERROR: Could not open camera 2. Exiting" );
    }
    cout << "Cameras opened suffessfully\n";

    Mat rFrameOld, rFrameNew, lFrameOld, lFrameNew, depthImg;
    vector<Point2f> pointsOld, pointsNew, goodPointsOld, goodPointsNew;
    vector<Point2f> directions;
    vector<ObjectPoint2f> objectPoints;
    vector<set<int>> objects;
    vector<unsigned char> status;
    vector<float> err;
    float objDepthFuzz = 0.1f;	//sample value. Will need tuning.

    pointsOld.reserve( NUM_POINTS );
    pointsNew.reserve( NUM_POINTS );
    status.reserve( NUM_POINTS );
    err.reserve( NUM_POINTS );

    StereoBM *stereoCompute = createStereoBM( 0, 13 );

    namedWindow( "Camera1", 1 );
    namedWindow( "Camera2", 2 );
    namedWindow( "Output", 3 );

    cam1 >> rFrameOld;
    cvtColor( rFrameOld, rFrameOld, COLOR_BGR2GRAY );
    goodFeaturesToTrack( rFrameOld, pointsOld, NUM_POINTS, QUALITY_LEVEL, 
        MIN_DISTANCE );
    
    while( true ) {
        //swap frames one and two, for purposes of object tracking
        rFrameNew = rFrameOld.clone();
        lFrameNew = lFrameOld.clone();
        cam1 >> rFrameOld;
        cam2 >> lFrameOld;

        cvtColor( rFrameOld, rFrameOld, COLOR_BGR2GRAY );
        cvtColor( lFrameOld, lFrameOld, COLOR_BGR2GRAY );

        calcOpticalFlowPyrLK( rFrameNew, rFrameOld, pointsNew, pointsOld, 
            status, err );

        goodPointsOld.clear();
        goodPointsNew.clear();

        objectPoints.clear();
        for( int i = 0; i < status.size(); i++ ) {
            if( status[i] > DESIRED_QUALITY ) {
                goodPointsOld.push_back( pointsOld[i] );
                goodPointsNew.push_back( pointsNew[i] );
                objectPoints.push_back( pointsNew[i] );
                //so now we have a list of all the good points and their
                //positions on this frame and the last
                //We should keep their movement vectors so we can calculate the
                //movement of a given object
                directions.push_back( goodPointsNew[i] - goodPointsOld[i] );
            }
        }
                
        stereoCompute->compute( lFrameOld, rFrameOld, depthImg );
        //depthImg is now the depth image, with a depth of 1 being no distance
        //and a depth of 0 being all the depth

        //We have the depth, we have the points we care about. We now need to
        //find which points go with which object
        //Enter recognition of an arbitrary object!
        
        //if point j is within objDepthFuzz depth value of point i, 
        //then point j is in the same object as point i. Each point 
        //needs to know which object it's in. Of course, this point 
        //concatenation only needs to happen when finding the points 
        //for the first time.
        //So, like, every ten frames maybe?

        int curObj = 0;

        objectPoints[0].obj = curObj;
        ObjectPoint2f *pointI, *pointJ;
        vector<ObjectPoint2f>::iterator begin = objectPoints.begin();
        for( int i = 0; i < objectPoints.size( ); i++ ) {
            pointI = &objectPoints[i];
            //has this point been added to any objects yet?
            if( pointI->obj != -1 ) {
                //if so, we can see if anything else needs to be added to this
                //object
                for( int j = i + 1; j < objectPoints.size(); j++ ) {
                    pointJ = &objectPoints[j];
                    float depthDiff = depthImg.at<short>( pointI->x, pointI->y ) -
                        depthImg.at<short>( pointJ->x, pointJ->y );
                    if( abs( depthDiff ) < objDepthFuzz ) {
                        pointJ->obj = pointI->obj;
                        //if this throws an error, the algorithm is wrong
                        objects[pointI->obj].insert( j );
                    }
                }
            } else {
                //if not, create a new object with this point in it
                objects.push_back( set<int>() );
                objects[curObj].emplace( i );
                curObj++;
            }
        }

        //at this point we know which direction vectors belong where
        //We can now go about 

        imshow( "Output", depthImg );

        if( waitKey( 30 ) >= 0 ) {
            break;
        }
    }
    return 0;
}