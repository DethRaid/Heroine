#include "ObjectPoint3f.h"

ObjectPoint3f::ObjectPoint3f() : Point3f() {
    x = y = 0;
    obj = -1;
}

ObjectPoint3f::ObjectPoint3f( float _x, float _y, float _z ) :
    Point3f( _x, _y, _z ) {
    obj = -1;
}

ObjectPoint3f::ObjectPoint3f( const ObjectPoint3f &pt ) : ObjectPoint3f( ) {
    x = pt.x;
    y = pt.y;
    z = pt.z;
    obj = pt.obj;
}

ObjectPoint3f& ObjectPoint3f::operator=(ObjectPoint3f pt) {
    swap( *this, pt );
    return *this;
}

ObjectPoint3f& ObjectPoint3f::operator=(Point3f pt) {
    swap( *this, pt );
    return *this;
}

ObjectPoint3f& ObjectPoint3f::operator-=(Point3f &rhs) {
    x -= rhs.x;
    z -= rhs.z;
    y -= rhs.y;
    return *this;
}

ObjectPoint3f& ObjectPoint3f::operator/=(float f) {
    x /= f;
    y /= f;
    z /= f;
    return *this;
}

ObjectPoint3f operator/(ObjectPoint3f lhs, float f) {
    lhs /= f;
    return lhs;
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

void ObjectPoint3f::swap( ObjectPoint3f &pt1, Point3f &pt2 ) {
    float temp = pt2.x;
    pt2.x = pt1.x;
    pt1.x = temp;

    temp = pt2.y;
    pt2.y = pt1.y;
    pt1.y = temp;

    temp = pt2.z;
    pt2.z = pt1.z;
    pt1.z = temp;
}