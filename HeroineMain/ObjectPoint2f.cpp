#include "ObjectPoint2f.h"

ObjectPoint2f::ObjectPoint2f() {
    x = y = obj = 0;
}

ObjectPoint2f::ObjectPoint2f( _Tp _x, _Tp _y ) {
    x = _x;
    y = _y;
}

ObjectPoint2f::ObjectPoint2f( const ObjectPoint2f &pt ) {
    x = pt.x;
    y = pt.y;
    obj = pt.obj;
}

ObjectPoint2f::ObjectPoint2f( const Point2f &pt ) : Point_( pt ) {}

ObjectPoint2f::ObjectPoint2f( const CvPoint &pt ) : Point_( pt ) {}

ObjectPoint2f::ObjectPoint2f( const CvPoint2D32f &pt ) : Point_( pt ) {}

ObjectPoint2f::ObjectPoint2f( const Size_<_Tp> &sz ) : Point_( sz ) {}

ObjectPoint2f::ObjectPoint2f( const Vec<_Tp, 2> &v ) : Point_( v ) {}

ObjectPoint2f& ObjectPoint2f::operator=(ObjectPoint2f pt) {
    swap( *this, pt );
    return *this;
}

ObjectPoint2f& ObjectPoint2f::operator=(Point2f pt) {
    swap( *this, pt );
    return *this;
}

void ObjectPoint2f::swap( ObjectPoint2F &pt1, ObjectPoint2f &pt2 ) {
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

void ObjectPoint2f::swap( ObjectPoint2F &pt1, Point2f &pt2 ) {
    float temp = pt2.x;
    pt2.x = pt1.x;
    pt1.x = temp;

    temp = pt2.y;
    pt2.y = pt1.y;
    pt1.y = temp;
}