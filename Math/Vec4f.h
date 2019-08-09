#pragma once

#include <math.h>

class Vec3f;

class Vec4f
{
public:
    Vec4f()
    {
        set( 0, 0, 0, 0 );
    }

    Vec4f( const Vec4f& other )
    {
        *this = other;
    }

    Vec4f( float v[4] )
    {
        this->set( v[0], v[1], v[2], v[3] );
    }

    Vec4f( float v0, float v1, float v2, float v3 )
    {
        this->set( v0, v1, v2, v3 );
    }

    Vec4f& operator=( const Vec4f& other )
    {
        this->set( other.v[0], other.v[1], other.v[2], other.v[3] );
        return *this;
    }

    Vec4f& operator=( const Vec3f& other );

    void set( float v0, float v1, float v2, float v3 )
    {
        this->v[0] = v0;
        this->v[1] = v1;
        this->v[2] = v2;
        this->v[3] = v3;
    }

    bool equal( const Vec4f& other, float tol ) const
    {
        return fabs( v[0] - other.v[0] ) < tol &&
            fabs( v[1] - other.v[1] ) < tol &&
            fabs( v[2] - other.v[2] ) < tol &&
            fabs( v[3] - other.v[3] ) < tol;
    }

    float& operator[]( int i )
    {
        return v[i];
    }

    const float& operator[]( int i ) const
    {
        return v[i];
    }

    Vec4f operator+( const Vec4f &other ) const
    {
        return Vec4f( v[0] + other.v[0],
            v[1] + other.v[1],
            v[2] + other.v[2],
            v[3] + other.v[3] );
    }

    Vec4f operator-( const Vec4f &other ) const
    {
        return Vec4f( v[0] - other.v[0],
            v[1] - other.v[1],
            v[2] - other.v[2],
            v[3] - other.v[3] );
    }

    Vec4f operator-( void ) const
    {
        return Vec4f( -v[0], -v[1], -v[2], -v[3] );
    }

    Vec4f operator*( float scale ) const
    {
        return Vec4f( v[0] * scale,
            v[1] * scale,
            v[2] * scale,
            v[3] * scale );
    }

    Vec4f operator/( float scale ) const
    {
        return Vec4f( v[0] / scale,
            v[1] / scale,
            v[2] / scale,
            v[3] / scale );
    }

    Vec4f& operator+=( const Vec4f &other )
    {
        v[0] += other.v[0];
        v[1] += other.v[1];
        v[2] += other.v[2];
        v[3] += other.v[3];
        return *this;
    }

    Vec4f& operator-=( const Vec4f &other )
    {
        v[0] -= other.v[0];
        v[1] -= other.v[1];
        v[2] -= other.v[2];
        v[3] -= other.v[3];
        return *this;
    }

    Vec4f& operator*=( float scale )
    {
        v[0] *= scale;
        v[1] *= scale;
        v[2] *= scale;
        v[3] *= scale;
        return *this;
    }

    Vec4f& operator/=( float scale )
    {
        v[0] /= scale;
        v[1] /= scale;
        v[2] /= scale;
        v[3] /= scale;
        return *this;
    }

    float dot( const Vec4f &other ) const
    {
        return v[0] * other.v[0] + v[1] * other.v[1] + v[2] * other.v[2] + v[3] * other.v[3];
    }

    float operator*( const Vec4f &other ) const
    {
        return dot( other );
    }

    float length() const
    {
        return sqrtf( length2() );
    }

    float length2() const
    {
        return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
    }

    void normalize()
    {
        *this /= length();
    }

    Vec4f normalized() const
    {
        return *this / length();
    }

    bool isZero( float tol ) const
    {
        return ( fabsf( v[0] ) < tol ) && ( fabsf( v[1] ) < tol ) && ( fabsf( v[2] ) < tol ) && ( fabsf( v[3] ) < tol );
    }

    float angle( const Vec4f& other ) const
    {
        return acosf( dot( other ) / this->length() * other.length() );
    }

private:
    float v[4];
};