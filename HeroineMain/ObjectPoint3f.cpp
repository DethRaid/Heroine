#include "ObjectPoint3f.h"

ObjectPoint3f::ObjectPoint3f() : Point2f() {
    x = y = 0;
    obj = -1;
}

ObjectPoint3f::ObjectPoint3f( float _x, float _y, float _z ) {
    x = _x;
    y = _y;
    z = _z;
    obj = -1;
}

ObjectPoint3f::ObjectPoint3f( const ObjectPoint3f &pt ) : ObjectPoint3f( ) {
    x = pt.x;
    y = pt.y;
    obj = pt.obj;
}

ObjectPoint3f::ObjectPoint3f( const Point2f &pt ) {
    x = pt.x;
    y = pt.y;
    obj = -1;
}

ObjectPoint3f::ObjectPoint3f( const CvPoint &pt ) {
    x = pt.x;
    y = pt.y;
    obj = -1;
}

ObjectPoint3f::ObjectPoint3f( const CvPoint2D32f &pt ) {
    x = pt.x;
    y = pt.y;
    obj = -1;
}

ObjectPoint3f::ObjectPoint3f( const Size_<float> &sz ) {
    x = sz.width;
    y = sz.height;
    obj = -1;
}

ObjectPoint3f::ObjectPoint3f( const Vec<float, 2> &v ) {
    x = v[0];
    y = v[1];
    obj = -1;
}

ObjectPoint3f& ObjectPoint3f::operator=(ObjectPoint3f pt) {
    swap( *this, pt );
    return *this;
}

ObjectPoint3f& ObjectPoint3f::operator=(Point2f pt) {
    swap( *this, pt );
    return *this;
}

void ObjectPoint3f::swap( ObjectPoint3f &pt1, ObjectPoint3f &pt2 ) {
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

void ObjectPoint3f::swap( ObjectPoint3f &pt1, Point2f &pt2 ) {
    float temp = pt2.x;
    pt2.x = pt1.x;
    pt1.x = temp;

    temp = pt2.y;
    pt2.y = pt1.y;
    pt1.y = temp;
}