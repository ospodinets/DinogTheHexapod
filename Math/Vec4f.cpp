#include "Vec4f.h"
#include "Vec3f.h"

Vec4f & Vec4f::operator=( const Vec3f & other )
{
    this->set( other[0], other[1], other[2], 1.0 );
    return *this;
}
