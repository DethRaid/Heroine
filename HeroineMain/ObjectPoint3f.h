#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

using namespace cv;

class CV_EXPORTS ObjectPoint3f : public Point2f {
public:
    ObjectPoint3f();
    ObjectPoint3f( float _x, float _y, float _z );
    ObjectPoint3f( const ObjectPoint3f &pt );
    ObjectPoint3f( const Point2f &pt );
    ObjectPoint3f( const CvPoint &pt );
    ObjectPoint3f( const CvPoint2D32f &pt );
    ObjectPoint3f( const Size_<float> &sz );
    ObjectPoint3f( const Vec<float, 2> &v );

    ObjectPoint3f& operator=(Point2f pt);
    ObjectPoint3f& operator=(ObjectPoint3f pt);

    /*!\brief The index of the object this point belongs to.
    
    All objects are in an array, so they all have indexes*/
    int obj;

    float z;

    /*!\brief Should this point be tracked or not?
    
    If TRUE, this point is submitted to the tracking algorithm for fiinding in 
    the next frame. If it is false, we use this tracks's history and the 
    object's history to predict where it will be next frame.*/
    bool shouldTrack;
private:
    void swap( ObjectPoint3f &pt1, ObjectPoint3f &pt2 );
    void swap( ObjectPoint3f &pt1, Point2f &pt2 );
};