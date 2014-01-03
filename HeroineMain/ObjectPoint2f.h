#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

using namespace cv;

class CV_EXPORTS ObjectPoint2f : public Point2f {
public:
    ObjectPoint2f();
    ObjectPoint2f( float _x, float _y );
    ObjectPoint2f( const ObjectPoint2f &pt );
    ObjectPoint2f( const Point2f &pt );
    ObjectPoint2f( const CvPoint &pt );
    ObjectPoint2f( const CvPoint2D32f &pt );
    ObjectPoint2f( const Size_<float> &sz );
    ObjectPoint2f( const Vec<float, 2> &v );

    ObjectPoint2f& operator=(Point2f pt);
    ObjectPoint2f& operator=(ObjectPoint2f pt);

    /*!\brief The index of the object this point belongs to.
    
    All objects are in an array, so they all have indexes*/
    int obj;
private:
    void swap( ObjectPoint2f &pt1, ObjectPoint2f &pt2 );
    void swap( ObjectPoint2f &pt1, Point2f &pt2 );
};