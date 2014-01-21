#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

using namespace cv;

class CV_EXPORTS ObjectPoint3f : public Point3f {
public:
    ObjectPoint3f();
    ObjectPoint3f( float _x, float _y, float _z );
    ObjectPoint3f( const ObjectPoint3f &pt );

    ObjectPoint3f& operator=(Point3f pt);
    ObjectPoint3f& operator=(ObjectPoint3f pt);

    ObjectPoint3f& operator-=(Point3f &rhs);
    ObjectPoint3f& operator/=(float f);

    /*!\brief The index of the object this point belongs to.
    
    All objects are in an array, so they all have indexes*/
    int obj;

    /*!\brief Should this point be tracked or not?
    
    If TRUE, this point is submitted to the tracking algorithm for fiinding in 
    the next frame. If it is false, we use this tracks's history and the 
    object's history to predict where it will be next frame.*/
    bool shouldTrack;
private:
    void swap( ObjectPoint3f &pt1, ObjectPoint3f &pt2 );
    void swap( ObjectPoint3f &pt1, Point3f &pt2 );
};

ObjectPoint3f operator/(ObjectPoint3f lhs, float f);