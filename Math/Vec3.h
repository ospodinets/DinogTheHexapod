#pragma once

#include <math.h>

class Vec3f
{
public:
    Vec3f()
    {
        v[0] = v[1] = v[2] = 0;
    }

    Vec3f( const Vec3f& other )
    {
        *this = other;
    }

    Vec3f( float v[3] )
    {
        this->v[0] = v[0];
        this->v[1] = v[1];
        this->v[2] = v[2];
    }

    Vec3f( float v0, float v1, float v2 )
    {
        this->v[0] = v0;
        this->v[1] = v1;
        this->v[2] = v2;
    }

    Vec3f& operator=( const Vec3f& other )
    {
        this->v[0] = other.v[0];
        this->v[1] = other.v[1];
        this->v[2] = other.v[2];
        return *this;
    }

    bool equal( const Vec3f& other, float tol ) const
    {
        return fabs( v[0] - other.v[0] ) < tol &&
            fabs( v[1] - other.v[1] ) < tol &&
            fabs( v[1] - other.v[1] ) < tol;
    }

    float& operator[]( int i )
    {
        return v[i];
    }

    const float& operator[]( int i ) const
    {
        return v[i];
    }

    Vec3f operator+( const Vec3f &other ) const
    {
        return Vec3f( v[0] + other.v[0],
            v[1] + other.v[1],
            v[2] + other.v[2] );
    }

    Vec3f operator-( const Vec3f &other ) const
    {
        return Vec3f( v[0] - other.v[0],
            v[1] - other.v[1],
            v[2] - other.v[2] );
    }

    Vec3f operator-( void ) const
    {
        return Vec3f( -v[0], -v[1], -v[2] );
    }

    Vec3f operator*( float scale ) const
    {
        return Vec3f( v[0] * scale,
            v[1] * scale,
            v[2] * scale );
    }

    Vec3f operator/( float scale ) const
    {
        return Vec3f( v[0] / scale,
            v[1] / scale,
            v[2] / scale );
    }

    Vec3f& operator+=( const Vec3f &other )
    {
        v[0] += other.v[0];
        v[1] += other.v[1];
        v[2] += other.v[2];
        return *this;
    }

    Vec3f& operator-=( const Vec3f &other )
    {
        v[0] -= other.v[0];
        v[1] -= other.v[1];
        v[2] -= other.v[2];
        return *this;
    }

    Vec3f& operator*=( float scale )
    {
        v[0] *= scale;
        v[1] *= scale;
        v[2] *= scale;
        return *this;
    }

    Vec3f& operator/=( float scale )
    {
        v[0] /= scale;
        v[1] /= scale;
        v[2] /= scale;
        return *this;
    }

    float dot( const Vec3f &other ) const
    {
        return v[0] * other.v[0] + v[1] * other.v[1] + v[2] * other.v[2];
    }

    float operator*( const Vec3f &other ) const
    {
        return dot( other );
    }

    Vec3f cross( const Vec3f &other ) const
    {
        return Vec3f( v[1] * other.v[2] - v[2] * other.v[1],
            v[2] * other.v[0] - v[0] * other.v[2],
            v[0] * other.v[1] - v[1] * other.v[0] );
    }

    float length() const
    {
        return sqrtf( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
    }

    float length2() const
    {
        return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    }

    void normalize()
    {
        *this /= length();
    }

    Vec3f normalized() const
    {
        return *this / length();
    }

    bool isZero( float tol ) const
    {
        return ( fabsf( v[0] ) < tol ) && ( fabsf( v[1] ) < tol ) && ( fabsf( v[2] ) < tol );
    }

    float angle( const Vec3f& other ) const
    {
        return acosf( dot( other ) / this->length() * other.length() );
    }

private:
    float v[3];
};