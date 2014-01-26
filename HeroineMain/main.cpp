#include <cmath>
#include <ctime>

#include <fstream>

#include <forward_list>
#include <vector>

#include "ObjectPoint3f.h"
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

/* The code here will look ro significant points, group them together based on
 * depth, find the average position and velocity of each group, predict where
 * closest group will be by the time that the robot gets to it, then move to
 * avoid that position. This is the collision avoidance code.
 *
 * However, this code will do nothing to determine where Heroine is on-floor.
 * For that, we'll have to create a system of visual landmarks which Heroine can
 * identify, such as putting numbers on the wall next to doors.
 *
 * RIT has already done this for us, with the whiteboards. They all have room
 * numbers on them. However, I'm not sure if those numbers will be visible to
 * Heroine.

 * A second option is to use the other numbers that RIT provided, the ones on
 * the bottom of the */

/* Do some research to determine the appropriate value, Try to measure in 
   bananas */
#define ROBO_SPEED 20

/* Variables */
char frameCount;
fstream uart1;

VideoCapture rCam, lCam;
Mat rFrameOld, rFrameNew, lFrameOld, lFrameNew;
vector<unsigned char> status;
vector<float> err;
vector<ObjectPoint3f> worldPointsOld, worldPointsNew;
vector<Point2f> rPoints, lPoints;
vector<Point3f> objMovement, pointDirections;
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
void addSerialMessage( char msg );
Point3f &predictClosestObjPos();

int main() {
    frameCount = 0;
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

    time_t startTime = time( NULL );

    while( waitKey( max( int( 200 - (time( NULL ) - startTime) ), 2 ) ) < 0 ) {
        /* Idea: Fixed frames, because we probably can */
        startTime = time( NULL );
        getNextImages();

        /* Re-calculate the points every five frames (every second) to account
         * for objects entering ans exiting the visible area
         */
        if( frameCount % 5 == 0 ) {
            goodFeaturesToTrack( lFrameNew, lPoints, 500, 0.01, 10 );
        }

        findStereoPoints( );
        calculateWorldPoints();
        if( frameCount % 5 == 0 ) {
            worldPointsOld = worldPointsNew;
        }
        findFeatureMovement();

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

        Point3f predPos = predictClosestObjPos( );
        //we ARE the origin!
        if( abs( predPos.x ) < 50 ) {
            //The object will be within hitting distance (potentially)
            //Move to the right or left to avoid it.
            addSerialMessage( predPos.x < 0 ? 'r' : 'l' );
        }

        //Wait so that we only run at 5 FPS
        //_sleep( max( int(200 - (time( NULL ) - startTime)), 0 ) );

        display( ); 
        frameCount++;
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
    pointDirections.clear( );
    for( int i = 0; i < worldPointsNew.size(); i++ ) {
        pointDirections.push_back( worldPointsNew[i] - worldPointsOld[i] );
    }
}

void findStereoPoints() {
    calcOpticalFlowPyrLK( lFrameNew, rFrameNew, lPoints, rPoints,
        status, err );
}

void calculateWorldPoints() {
    float depth;
    worldPointsOld = worldPointsNew;    //Hopefully this is a copy-by-value op
    worldPointsNew.clear();
    for( int i = 0; i < rPoints.size(); i++ ) {
        //Z = Bf/(x - x')
        //B = 6.35 cm (about distance between human eyes)
        //   B = distance between cameras
        //f = 700 (units of some sort? OpenCV claims this is in pixels)
        //  Possibly this is in sensor pixels? Odd unit
        //  f = focal length of camera (it should be the same for both)
        //Bf = 4445
        //  It's just math
        depth = 4445 / (lPoints[i].x - rPoints[i].x);

        //We now have the points we can track on both images. Time to whip out 
        //our robes and wizard hats!

        //Divide everything by z so that a point farther away will be closer to
        //the origin for a given (u, v)
        worldPointsNew.push_back( ObjectPoint3f( lPoints[i].x / depth,
            lPoints[i].y / depth, depth ) );
    }
}

void findObjects( ) {
    int curObj = 0;
    ObjectPoint3f *pointI, *pointJ;
    for( int i = 0; i < worldPointsNew.size( ); i++ ) {
        pointI = &worldPointsNew[i];
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
        for( int j = i + 1; j < worldPointsNew.size( ); j++ ) {
            pointJ = &worldPointsNew[j];
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
    objMovement.clear();
    avgObjPos.clear();

    objMovement.reserve( objects.size() );
    avgObjPos.reserve( objects.size() );

    float count = 0;
    ObjectPoint3f pos;
    Point3f dir;
    for( int i = 0; i < objects.size(); i++ ) { //for each object
        for( int j : objects[i] ) {  //for each point in the object
            pos += worldPointsNew[j];
            dir += pointDirections[j];
            count += 1;
        }
        objMovement.push_back( Point3f( dir.x / count, dir.y / count,
            dir.z / count ) );
        avgObjPos.push_back( pos / count );
    }
}

Point3f &predictClosestObjPos() {
    int closestPos = 0;
    for( int i = 0; i < avgObjPos.size(); i++ ) {
        if( avgObjPos[i].z < avgObjPos[closestPos].z ) {
            closestPos = i;
        }
    }
    /* v = d/t
    tv = d
    t = d/v
    */
    float timeToClose = avgObjPos[closestPos].z / ROBO_SPEED;
    return objMovement[closestPos] * timeToClose;
}

void addSerialMessage( char msg ) {
    cout << msg << "\n";
    if( uart1.is_open() ) {
        uart1 << msg;
    }
}

void display() {
    imshow( "Right", rFrameNew );
    imshow( "Left", lFrameNew );
}