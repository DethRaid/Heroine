#include "ObjectPoint2f.h"

ObjectPoint2f::ObjectPoint2f() : Point2f() {
    x = y = 0;
    obj = -1;
}

ObjectPoint2f::ObjectPoint2f( float _x, float _y ) : ObjectPoint2f() {
    x = _x;
    y = _y;
}

ObjectPoint2f::ObjectPoint2f( const ObjectPoint2f &pt ) : ObjectPoint2f( ) {
    x = pt.x;
    y = pt.y;
    obj = pt.obj;
}

ObjectPoint2f::ObjectPoint2f( const Point2f &pt ) : Point2f( pt ) {
    obj = -1;
}

ObjectPoint2f::ObjectPoint2f( const CvPoint &pt ) : Point2f( pt ) {
    obj = -1;
}

ObjectPoint2f::ObjectPoint2f( const CvPoint2D32f &pt ) : Point2f( pt ) {
    obj = -1;
}

ObjectPoint2f::ObjectPoint2f( const Size_<float> &sz ) : Point2f( sz ) {
    obj = -1;
}

ObjectPoint2f::ObjectPoint2f( const Vec<float, 2> &v ) : Point2f( v ) {
    obj = -1;
}

ObjectPoint2f& ObjectPoint2f::operator=(ObjectPoint2f pt) {
    swap( *this, pt );
    return *this;
}

ObjectPoint2f& ObjectPoint2f::operator=(Point2f pt) {
    swap( *this, pt );
    return *this;
}

void ObjectPoint2f::swap( ObjectPoint2f &pt1, ObjectPoint2f &pt2 ) {
    float temp = pt2.x;
    pt2.x = pt1.x;
    pt1.x = temp;

    temp = pt2.y;
    pt2.y = pt1.y;
    pt1.y = temp;

    int tempi = pt2.obj;
    pt2.obj = pt1.obj;
    pt1.obj = temp;
}

void ObjectPoint2f::swap( ObjectPoint2f &pt1, Point2f &pt2 ) {
    float temp = pt2.x;
    pt2.x = pt1.x;
    pt1.x = temp;

    temp = pt2.y;
    pt2.y = pt1.y;
    pt1.y = temp;
}