#pragma once

#include "Vec4f.h"
#include "Vec3f.h"

class Mat4x4
{
public:
    Mat4x4()
    {
        set( 1.0 );
    }

    Mat4x4( float v )
    {
        set( v );
    }

    Mat4x4( const Mat4x4& other )
    {
        *this = other;
    }

    Mat4x4( const Vec4f m[4] )
    {
        this->set( m[0], m[1], m[2], m[3] );
    }

    Mat4x4( const Vec4f& m0,
        const Vec4f& m1,
        const Vec4f& m2,
        const Vec4f& m3 )
    {
        this->set( m0, m1, m2, m3 );
    }

    Mat4x4& operator=( const Mat4x4& other )
    {
        this->set( other.m[0], other.m[1], other.m[2], other.m[3] );
        return *this;
    }

    void set( const Vec4f& m0,
        const Vec4f& m1,
        const Vec4f& m2,
        const Vec4f& m3 )
    {
        this->m[0] = m0;
        this->m[1] = m1;
        this->m[2] = m2;
        this->m[3] = m3;
    }


    // sets matrix diagonal to the given value
    // and other values to 0.0
    void set( float v )
    {
        m[0].set( v, 0.0, 0.0, 0.0 );
        m[1].set( 0.0, v, 0.0, 0.0 );
        m[2].set( 0.0, 0.0, v, 0.0 );
        m[3].set( 0.0, 0.0, 0.0, v );
    }

    Vec4f& operator[]( int row )
    {
        return m[row];
    }

    const Vec4f& operator[]( int row ) const
    {
        return m[row];
    }

    void mult( const Mat4x4& rhs )
    {
        set( Vec4f( m[0][0] * rhs[0][0] + m[0][1] * rhs[1][0] + m[0][2] * rhs[2][0] + m[0][3] * rhs[3][0],
            m[0][0] * rhs[0][1] + m[0][1] * rhs[1][1] + m[0][2] * rhs[2][1] + m[0][3] * rhs[3][1],
            m[0][0] * rhs[0][2] + m[0][1] * rhs[1][2] + m[0][2] * rhs[2][2] + m[0][3] * rhs[3][2],
            m[0][0] * rhs[0][3] + m[0][1] * rhs[1][3] + m[0][2] * rhs[2][3] + m[0][3] * rhs[3][3] ),

            Vec4f( m[1][0] * rhs[0][0] + m[1][1] * rhs[1][0] + m[1][2] * rhs[2][0] + m[1][3] * rhs[3][0],
                m[1][0] * rhs[0][1] + m[1][1] * rhs[1][1] + m[1][2] * rhs[2][1] + m[1][3] * rhs[3][1],
                m[1][0] * rhs[0][2] + m[1][1] * rhs[1][2] + m[1][2] * rhs[2][2] + m[1][3] * rhs[3][2],
                m[1][0] * rhs[0][3] + m[1][1] * rhs[1][3] + m[1][2] * rhs[2][3] + m[1][3] * rhs[3][3] ),

            Vec4f( m[2][0] * rhs[0][0] + m[2][1] * rhs[1][0] + m[2][2] * rhs[2][0] + m[2][3] * rhs[3][0],
                m[2][0] * rhs[0][1] + m[2][1] * rhs[1][1] + m[2][2] * rhs[2][1] + m[2][3] * rhs[3][1],
                m[2][0] * rhs[0][2] + m[2][1] * rhs[1][2] + m[2][2] * rhs[2][2] + m[2][3] * rhs[3][2],
                m[2][0] * rhs[0][3] + m[2][1] * rhs[1][3] + m[2][2] * rhs[2][3] + m[2][3] * rhs[3][3] ),

            Vec4f( m[3][0] * rhs[0][0] + m[3][1] * rhs[1][0] + m[3][2] * rhs[2][0] + m[3][3] * rhs[3][0],
                m[3][0] * rhs[0][1] + m[3][1] * rhs[1][1] + m[3][2] * rhs[2][1] + m[3][3] * rhs[3][1],
                m[3][0] * rhs[0][2] + m[3][1] * rhs[1][2] + m[3][2] * rhs[2][2] + m[3][3] * rhs[3][2],
                m[3][0] * rhs[0][3] + m[3][1] * rhs[1][3] + m[3][2] * rhs[2][3] + m[3][3] * rhs[3][3] ) );
    }

    void premult( const Mat4x4 lhs )
    {
        auto& rhs = *this;
        set( Vec4f( lhs[0][0] * rhs[0][0] + lhs[0][1] * rhs[1][0] + lhs[0][2] * rhs[2][0] + lhs[0][3] * rhs[3][0],
            lhs[0][0] * rhs[0][1] + lhs[0][1] * rhs[1][1] + lhs[0][2] * rhs[2][1] + lhs[0][3] * rhs[3][1],
            lhs[0][0] * rhs[0][2] + lhs[0][1] * rhs[1][2] + lhs[0][2] * rhs[2][2] + lhs[0][3] * rhs[3][2],
            lhs[0][0] * rhs[0][3] + lhs[0][1] * rhs[1][3] + lhs[0][2] * rhs[2][3] + lhs[0][3] * rhs[3][3] ),

            Vec4f( lhs[1][0] * rhs[0][0] + lhs[1][1] * rhs[1][0] + lhs[1][2] * rhs[2][0] + lhs[1][3] * rhs[3][0],
                lhs[1][0] * rhs[0][1] + lhs[1][1] * rhs[1][1] + lhs[1][2] * rhs[2][1] + lhs[1][3] * rhs[3][1],
                lhs[1][0] * rhs[0][2] + lhs[1][1] * rhs[1][2] + lhs[1][2] * rhs[2][2] + lhs[1][3] * rhs[3][2],
                lhs[1][0] * rhs[0][3] + lhs[1][1] * rhs[1][3] + lhs[1][2] * rhs[2][3] + lhs[1][3] * rhs[3][3] ),

            Vec4f( lhs[2][0] * rhs[0][0] + lhs[2][1] * rhs[1][0] + lhs[2][2] * rhs[2][0] + lhs[2][3] * rhs[3][0],
                lhs[2][0] * rhs[0][1] + lhs[2][1] * rhs[1][1] + lhs[2][2] * rhs[2][1] + lhs[2][3] * rhs[3][1],
                lhs[2][0] * rhs[0][2] + lhs[2][1] * rhs[1][2] + lhs[2][2] * rhs[2][2] + lhs[2][3] * rhs[3][2],
                lhs[2][0] * rhs[0][3] + lhs[2][1] * rhs[1][3] + lhs[2][2] * rhs[2][3] + lhs[2][3] * rhs[3][3] ),

            Vec4f( lhs[3][0] * rhs[0][0] + lhs[3][1] * rhs[1][0] + lhs[3][2] * rhs[2][0] + lhs[3][3] * rhs[3][0],
                lhs[3][0] * rhs[0][1] + lhs[3][1] * rhs[1][1] + lhs[3][2] * rhs[2][1] + lhs[3][3] * rhs[3][1],
                lhs[3][0] * rhs[0][2] + lhs[3][1] * rhs[1][2] + lhs[3][2] * rhs[2][2] + lhs[3][3] * rhs[3][2],
                lhs[3][0] * rhs[0][3] + lhs[3][1] * rhs[1][3] + lhs[3][2] * rhs[2][3] + lhs[3][3] * rhs[3][3] ) );
    }

    Vec4f mult( const Vec4f &p )
    {
        return
            Vec4f(
                m[0][0] * p[0] + m[1][0] * p[1] + m[2][0] * p[2] + m[3][0] * p[3],
                m[0][1] * p[0] + m[1][1] * p[1] + m[2][1] * p[2] + m[3][1] * p[3],
                m[0][2] * p[0] + m[1][2] * p[1] + m[2][2] * p[2] + m[3][2] * p[3],
                m[0][3] * p[0] + m[1][3] * p[1] + m[2][3] * p[2] + m[3][3] * p[3]
            );
    }

    Vec3f mult( const Vec3f &p )
    {
        return
            Vec3f(
                m[0][0] * p[0] + m[1][0] * p[1] + m[2][0] * p[2] + m[3][0],
                m[0][1] * p[0] + m[1][1] * p[1] + m[2][1] * p[2] + m[3][1],
                m[0][2] * p[0] + m[1][2] * p[1] + m[2][2] * p[2] + m[3][2]
            );
    }    

    float det()
    {
        return
            m[0][3] * m[1][2] * m[2][1] * m[3][0] - m[0][2] * m[1][3] * m[2][1] * m[3][0] - m[0][3] * m[1][1] * m[2][2] * m[3][0] + m[0][1] * m[1][3] * m[2][2] * m[3][0] +
            m[0][2] * m[1][1] * m[2][3] * m[3][0] - m[0][1] * m[1][2] * m[2][3] * m[3][0] - m[0][3] * m[1][2] * m[2][0] * m[3][1] + m[0][2] * m[1][3] * m[2][0] * m[3][1] +
            m[0][3] * m[1][0] * m[2][2] * m[3][1] - m[0][0] * m[1][3] * m[2][2] * m[3][1] - m[0][2] * m[1][0] * m[2][3] * m[3][1] + m[0][0] * m[1][2] * m[2][3] * m[3][1] +
            m[0][3] * m[1][1] * m[2][0] * m[3][2] - m[0][1] * m[1][3] * m[2][0] * m[3][2] - m[0][3] * m[1][0] * m[2][1] * m[3][2] + m[0][0] * m[1][3] * m[2][1] * m[3][2] +
            m[0][1] * m[1][0] * m[2][3] * m[3][2] - m[0][0] * m[1][1] * m[2][3] * m[3][2] - m[0][2] * m[1][1] * m[2][0] * m[3][3] + m[0][1] * m[1][2] * m[2][0] * m[3][3] +
            m[0][2] * m[1][0] * m[2][1] * m[3][3] - m[0][0] * m[1][2] * m[2][1] * m[3][3] - m[0][1] * m[1][0] * m[2][2] * m[3][3] + m[0][0] * m[1][1] * m[2][2] * m[3][3];
    }

    void transpose()
    {
        auto swap = []( float& a, float& b )
        {
            float t = a;
            a = b;
            b = t;
        };
        swap( m[0][1], m[1][0] );
        swap( m[0][2], m[2][0] );
        swap( m[0][3], m[3][0] );
        swap( m[1][2], m[1][1] );
        swap( m[1][3], m[2][1] );
        swap( m[2][3], m[3][1] );
    }

    static Mat4x4 translationMatrix( const Vec3f& translation )
    {
        Mat4x4 res( 1.0f );
        res[3] = translation;
        return res;
    }

    static Mat4x4 scaleMatrix( const Vec3f& scale )
    {
        Mat4x4 res( 1.0f );
        res[0][0] = scale[0];
        res[1][1] = scale[1];
        res[2][2] = scale[2];
        return res;
    }

private:
    Vec4f m[4];
};