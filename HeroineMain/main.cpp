#include <fstream>

#include <forward_list>
#include <vector>

#include "ObjectPoint3f.h"
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

/* Variables */
fstream uart1;

VideoCapture rCam, lCam;
Mat rFrameOld, rFrameNew, lFrameOld, lFrameNew;
vector<unsigned char> status;
vector<float> err;
vector<ObjectPoint3f> worldPoints;
vector<Point2f> rPoints, lPointsOld, lPointsNew, directions;
vector<Point2f> ssObjMovement;
vector<ObjectPoint3f> avgObjPos;
vector<forward_list<int>> objects;

/* Forward declaration of functions */
int main();
void getNextImages();
void findFeatureMovement();
void findStereoPoints();
void calculateWorldPoints();
void findObjects();
void calcAvgPosAndDir();
void display();

int main() {
    /* Open video streams */
    rCam = VideoCapture( 0 );   //hopefully correct value
    lCam = VideoCapture( 1 );

    if( !rCam.isOpened() || !lCam.isOpened() ) {
        cout << "ERROR: One or both cameras not available. Check that they are"
            << " plugged in, please\n";
    }

    /* Open serial stream */
    uart1.open( "/dev/tty01" );
    if( !uart1.is_open() ) {
        cout << "ERROR: Could not open serial stream. Control functionality not"
            << " available\n";
    }

    /* Open windows */
    namedWindow( "Left", 1 );
    namedWindow( "Right", 2 );

    getNextImages();

    goodFeaturesToTrack( lFrameNew, lPointsNew, 500, 0.01, 10 );

    while( waitKey( 30 ) < 0 ) {
        getNextImages();

        lPointsOld = lPointsNew;
        calcOpticalFlowPyrLK( lFrameOld, lFrameNew, lPointsOld, lPointsNew,
            status, err );

        findStereoPoints();
        calculateWorldPoints();

        //So we now have the points in their world position, as well as last
        //frame's points in screen space

        //The screen-space points can tell us if an object is moving to the
        //right or left
        //Even better, we can get each object's average distance and, based on
        //their average movements, we can calculate where the objects will be
        //(Kinda) when we get to them
        //First, find the objects

        findObjects();
        calcAvgPosAndDir();

        display();
    }

    return 0;
}

void getNextImages() {
    rFrameOld = rFrameNew.clone();
    lFrameOld = lFrameNew.clone();

    rCam >> rFrameNew;
    lCam >> lFrameNew;

    cvtColor( rFrameNew, rFrameNew, COLOR_BGR2GRAY );
    cvtColor( lFrameNew, lFrameNew, COLOR_BGR2GRAY );
}

/* Find out where the feature films moved to */
void findFeatureMovement() {
    directions.clear( ); 
    for( int i = 0; i < lPointsNew.size(); i++ ) {
        directions.push_back( lPointsNew[i] - lPointsOld[i] );
    }
}

void findStereoPoints() {
    calcOpticalFlowPyrLK( lFrameNew, rFrameNew, lPointsOld, rPoints,
        status, err );
}

void calculateWorldPoints() {
    float depth;
    worldPoints.clear();
    for( int i = 0; i < rPoints.size(); i++ ) {
        //Z = Bf/(x - x')
        //B = 6.35 cm (about distance between human eyes)
        //   B = distance between cameras
        //f = 700 (units of some sort? OpenCV claims this is in pixels)
        //  Possibly this is in sensor pixels? Odd unit
        //  f = focal length of camera (it should be the same for both)
        //Bf = 4445
        //  It's just math
        depth = 4445 / (lPointsNew[i].x - rPoints[i].x);

        //We now have the points we can track on both images. Time to whip out 
        //our robes and wizard hats!

        //Divide everything by z so that a point farther away will be closer to
        //the origin for a given (u, v)
        worldPoints.push_back( ObjectPoint3f( lPointsNew[i].x / depth,
            lPointsNew[i].y / depth, depth ) );
    }
}

void findObjects( ) {
    int curObj = 0;
    ObjectPoint3f *pointI, *pointJ;
    for( int i = 0; i < worldPoints.size( ); i++ ) {
        pointI = &worldPoints[i];
        //has this point been added to any objects yet?
        if( pointI->obj == -1 ) {
            //if not, create a new object with this point in it
            objects.push_back( forward_list<int>( ) );
            objects[curObj].emplace_front( i );
            pointI->obj = curObj;
            curObj++;
        }
        //if so, we can see if anything else needs to be added to this
        //object
        for( int j = i + 1; j < worldPoints.size( ); j++ ) {
            pointJ = &worldPoints[j];
            float depthDiff = pointI->z - pointJ->z;
            if( abs( depthDiff ) < 40 ) {
                pointJ->obj = pointI->obj;
                //if this throws an error, the algorithm is wrong
                objects[pointI->obj].emplace_front( j );
            }
        }
    }
}

void calcAvgPosAndDir() {
    ssObjMovement.clear();
    avgObjPos.clear();

    ssObjMovement.reserve( objects.size() );
    avgObjPos.reserve( objects.size() );

    float count = 0;
    ObjectPoint3f pos;
    Point2f dir;
    for( int i = 0; i < objects.size(); i++ ) { //for each object
        for( int j : objects[i] ) {  //for each point in the object
            pos += worldPoints[j];
            dir += directions[j];
            count += 1;
        }
        ssObjMovement.push_back( Point2f( dir.x / count, dir.y / count ) );
        avgObjPos.push_back(  )
    }
}

void display() {
    imshow( "Right", rFrameNew );
    imshow( "Left", lFrameNew );
}