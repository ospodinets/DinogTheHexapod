#pragma once

#include "Vec4f.h"
#include "Vec3f.h"

class Mat3x3
{
public:
    Mat3x3()
    {
        set( 1.0 );
    }

    Mat3x3( float v )
    {
        set( v );
    }

    Mat3x3( const Mat3x3& other )
    {
        *this = other;
    }

    Mat3x3( const Vec3f m[3] )
    {
        this->set( m[0], m[1], m[2] );
    }

    Mat3x3( const Vec3f& m0,
        const Vec3f& m1,
        const Vec3f& m2 )
    {
        this->set( m0, m1, m2 );
    }

    Mat3x3& operator=( const Mat3x3& other )
    {
        this->set( other.m[0], other.m[1], other.m[2] );
        return *this;
    }

    void set( const Vec3f& m0,
        const Vec3f& m1,
        const Vec3f& m2 )
    {
        this->m[0] = m0;
        this->m[1] = m1;
        this->m[2] = m2;
    }


    // sets matrix diagonal to the given value
    // and other values to 0.0
    void set( float v )
    {
        m[0].set( v, 0.0, 0.0 );
        m[1].set( 0.0, v, 0.0 );
        m[2].set( 0.0, 0.0, v );
    }

    Vec3f& operator[]( int row )
    {
        return m[row];
    }

    const Vec3f& operator[]( int row ) const
    {
        return m[row];
    }

    Mat3x3 mult( const Mat3x3 rhs )
    {
        set(
            Vec3f( m[0][0] * rhs[0][0] + m[0][1] * rhs[1][0] + m[0][2] * rhs[2][0],
                   m[0][0] * rhs[0][1] + m[0][1] * rhs[1][1] + m[0][2] * rhs[2][1],
                   m[0][0] * rhs[0][2] + m[0][1] * rhs[1][2] + m[0][2] * rhs[2][2] ),

            Vec3f( m[1][0] * rhs[0][0] + m[1][1] * rhs[1][0] + m[1][2] * rhs[2][0],
                   m[1][0] * rhs[0][1] + m[1][1] * rhs[1][1] + m[1][2] * rhs[2][1],
                   m[1][0] * rhs[0][2] + m[1][1] * rhs[1][2] + m[1][2] * rhs[2][2] ),

            Vec3f( m[2][0] * rhs[0][0] + m[2][1] * rhs[1][0] + m[2][2] * rhs[2][0],
                   m[2][0] * rhs[0][1] + m[2][1] * rhs[1][1] + m[2][2] * rhs[2][1],
                   m[2][0] * rhs[0][2] + m[2][1] * rhs[1][2] + m[2][2] * rhs[2][2] )
        );
    }

    Mat3x3 premult( const Mat3x3 lhs )
    {
        set(
            Vec3f( lhs[0][0] * m[0][0] + lhs[0][1] * m[1][0] + lhs[0][2] * m[2][0],
                   lhs[0][0] * m[0][1] + lhs[0][1] * m[1][1] + lhs[0][2] * m[2][1],
                   lhs[0][0] * m[0][2] + lhs[0][1] * m[1][2] + lhs[0][2] * m[2][2] ),

            Vec3f( lhs[1][0] * m[0][0] + lhs[1][1] * m[1][0] + lhs[1][2] * m[2][0],
                   lhs[1][0] * m[0][1] + lhs[1][1] * m[1][1] + lhs[1][2] * m[2][1],
                   lhs[1][0] * m[0][2] + lhs[1][1] * m[1][2] + lhs[1][2] * m[2][2] ),

            Vec3f( lhs[2][0] * m[0][0] + lhs[2][1] * m[1][0] + lhs[2][2] * m[2][0],
                   lhs[2][0] * m[0][1] + lhs[2][1] * m[1][1] + lhs[2][2] * m[2][1],
                   lhs[2][0] * m[0][2] + lhs[2][1] * m[1][2] + lhs[2][2] * m[2][2] )
        );
    }

    Vec3f mult( const Vec3f &p )
    {
        return Vec3f(
            m[0][0] * p[0] + m[1][0] * p[1] + m[2][0] * p[2],
            m[0][1] * p[0] + m[1][1] * p[1] + m[2][1] * p[2],
            m[0][2] * p[0] + m[1][2] * p[1] + m[2][2] * p[2]
        );
    }

    float det() const
    {
        return m[0][0] * ( m[1][1] * m[2][2] - m[1][2] * m[2][1] ) +
               m[0][1] * ( m[1][2] * m[2][0] - m[1][0] * m[2][2] ) +
               m[0][2] * ( m[1][0] * m[2][1] - m[1][1] * m[2][0] );
    }

    bool inverse()
    {
        auto d = det();

        if( fabs( d ) < 1e-06 )
        {
            return false;
        }

        set( Vec3f ( ( m[1][1] * m[2][2] - m[2][1] * m[1][2] ) / d,
                     ( m[0][2] * m[2][1] - m[0][1] * m[2][2] ) / d,
                     ( m[0][1] * m[1][2] - m[0][2] * m[1][1] ) / d ),
             Vec3f ( ( m[1][2] * m[2][0] - m[1][0] * m[2][2] ) / d,
                     ( m[0][0] * m[2][2] - m[0][2] * m[2][0] ) / d,
                     ( m[1][0] * m[0][2] - m[0][0] * m[1][2] ) / d ),
             Vec3f ( ( m[1][0] * m[2][1] - m[2][0] * m[1][1] ) / d,
                     ( m[2][0] * m[0][1] - m[0][0] * m[2][1] ) / d,
                     ( m[0][0] * m[1][1] - m[1][0] * m[0][1] ) / d ) );
        return true;
    }

private:
    Vec3f m[3];
};

