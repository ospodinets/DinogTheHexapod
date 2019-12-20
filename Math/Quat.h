#pragma once

#include "Vec3f.h"
#include "Mat4x4.h"
#include "Mat3x3.h"

class Quat
{
public:
    Quat()
    {
        set( 0, 0, 0, 1 );
    }

    Quat( const Quat& other )
    {
        *this = other;
    }

    Quat( const float q[4] )
    {
        this->set( q[0], q[1], q[2], q[3] );
    }

    Quat( float q0, float q1, float q2, float q3 )
    {
        this->set( q0, q1, q2, q3 );
    }

    Quat( const Vec3f &axis, float angle )
    {
        float ahalf = angle * 0.5f;
        float a = cosf( ahalf );
        float b = sinf( ahalf );

        set( axis[0] * b, axis[1] * b, axis[2] * b, a );
    }

    Quat& operator=( const Quat& other )
    {
        this->set( other.q[0], other.q[1], other.q[2], other.q[3] );
        return *this;
    }

    void set( float q0, float q1, float q2, float q3 )
    {
        this->q[0] = q0;
        this->q[1] = q1;
        this->q[2] = q2;
        this->q[3] = q3;
    }

    float& operator[]( int i )
    {
        return q[i];
    }

    const float& operator[]( int i ) const
    {
        return q[i];
    }

    void mul( const Quat& other )
    {
        set(
            q[3] * other[0] + q[0] * other[3] + q[1] * other[2] - q[2] * other[1],
            q[3] * other[1] + q[1] * other[3] + q[2] * other[0] - q[0] * other[2],
            q[3] * other[2] + q[2] * other[3] + q[0] * other[1] - q[1] * other[0],
            q[3] * other[3] - q[0] * other[0] - q[1] * other[1] - q[2] * other[2]
        );
    }

    void normalize()
    {
        float l = sqrt( dot( q ) );

        if( l == 0.0f )
            set( 0.f, 0.f, 0.f, 1.f );
        else
        {
            float c = 1.0f / l;
            set( q[0] * c, q[1] * c, q[2] * c, q[3] * c );
        }
    }

    void inverse()
    {
        float l = dot( q );
        if( l == 0.0f )
            l = 1.0f;
        float c = -1.0f / l;
        set( q[0] * c, q[1] * c, q[2] * c, q[3] * ( -c ) );
    }

    void conjugate()
    {
        set( -q[0], -q[1], -q[2], q[0] );
    }

    float dot( const Quat& other ) const
    {
        return q[0] * other[0] + q[1] * other[1] + q[2] * other[2] + q[3] * other[3];
    }

    Vec3f rotate( const Vec3f &v )
    {
        Quat vq { v[0], v[1], v[2], 0.0f };
        Quat res( q );
        Quat cq( q );
        cq.conjugate();
        res.mul( vq );
        res.mul( cq );
        return Vec3f( res[0], res[1], res[2] );
    }

    Mat4x4 toMatrix() const
    {        
        const float	d = dot( q );
        float s;
        if( d )
            s = 2.0f / d;
        else
            s = 1.0f;

        const float	xs = q[0] * s;
        const float	ys = q[1] * s;
        const float	zs = q[2] * s;
        const float wx = q[3] * xs;
        const float wy = q[3] * ys;
        const float wz = q[3] * zs;
        const float xx = q[0] * xs;
        const float xy = q[0] * ys;
        const float xz = q[0] * zs;
        const float yy = q[1] * ys;
        const float yz = q[1] * zs;
        const float zz = q[2] * zs;

        return { Vec4f { ( 1.0f - yy - zz ), ( xy + wz ), ( xz - wy ), 0.0f },
                 Vec4f { ( xy - wz ), ( 1.0f - xx - zz ), ( yz + wx ), 0.0f },
                 Vec4f { ( xz + wy ), ( yz - wx ), ( 1.0f - xx - yy ), 0.0f },
                 Vec4f { 0.f, 0.f, 0.f, 1.f } };
    }

    Mat3x3 toMatrix3x3() const
    {
        const float	d = dot( q );
        float s;
        if( d )
            s = 2.0f / d;
        else
            s = 1.0f;

        const float	xs = q[0] * s;
        const float	ys = q[1] * s;
        const float	zs = q[2] * s;
        const float wx = q[3] * xs;
        const float wy = q[3] * ys;
        const float wz = q[3] * zs;
        const float xx = q[0] * xs;
        const float xy = q[0] * ys;
        const float xz = q[0] * zs;
        const float yy = q[1] * ys;
        const float yz = q[1] * zs;
        const float zz = q[2] * zs;

        return { 
            Vec3f { ( 1.0f - yy - zz ), ( xy + wz ), ( xz - wy ) },
            Vec3f { ( xy - wz ), ( 1.0f - xx - zz ), ( yz + wx ) },
            Vec3f { ( xz + wy ), ( yz - wx ), ( 1.0f - xx - yy ) }
        };
            
    }

private:
    float q[4];

};
