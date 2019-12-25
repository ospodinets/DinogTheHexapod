#pragma once

#include <MathUtils.h>

template< class T >
class LineSegment
{
public:
    LineSegment()
    {
    }

    LineSegment( const LineSegment& other )
    {
        *this = other;
    }

    LineSegment& operator=( const LineSegment& other )
    {
        m_v[0] = other.m_v[0];
        m_v[1] = other.m_v[1];
        m_t[0] = other.m_t[0];
        m_t[1] = other.m_t[1];
        m_slope = other.m_slope;
        m_domain[0] = other.m_domain[0];
        m_domain[1] = other.m_domain[1];
        m_trimmed = other.m_trimmed;
        m_mode = other.m_mode;
    }

    LineSegment( float slope, const T& v0, float t0 )
    {
        set( slope, v0, t0 );
    }    

    LineSegment( const T& v0, const T& v1, float t0, float t1, bool trimmed = true )
    {
        set( v0, v1, t0, t1 );
        if( trimmed )
            setTrim( t0, t1 );
    }

    void set( const T& v0, const T& v1, float t0, float t1 )
    {
        m_mode = TwoPoints;
        m_v[0] = v0;
        m_v[1] = v1;
        m_t[0] = t0;
        m_t[1] = t1;
    }

    void set( float slope, const T& v0, float t0 )
    {
        m_mode = Slope;
        m_slope = slope;
        m_v[0] = v0;        
        m_t[0] = t0;
    }

    T evaluate( float t ) const
    {
        if( m_trimmed )
        {
            t = max( t, m_domain[0] );
            t = min( t, m_domain[1] );
        }   

        T res {};

        if( m_mode == TwoPoints )
        {
            if( fabs( m_t[0] - m_t[1] ) > F_TOLERANCE )
                res = lerp( m_v[0], m_v[1], m_t[0], m_t[1], t );

        }            
        else
        {
            res = m_slope * ( t - m_t[0] ) + m_v[0];
        }   
        return res;
    }

    bool findT( const T& value, float& t )
    {
        if( m_mode == TwoPoints )
        {
            if( fabs(m_v[0] - m_v[1]) > F_TOLERANCE )
            {
                t = lerp( m_t[0], m_t[1], m_v[0], m_v[1], value );
                return true;
            }
        }            
        else
        {
            if( fabs(m_slope) > F_TOLERANCE )
            {
                t = ( value - m_v[0] ) / m_slope + m_t[0];
                return true;
            }
        }
        return false;
    }

    void setTrim( float t0, float t1 )
    {
        m_trimmed = true;
        m_domain[0] = min( t0, t1 );
        m_domain[1] = max( t0, t1 );
    }

    void removeTrim()
    {
        m_trimmed = false;
    }

    float domainMin() const
    {
        return m_trimmed ? m_domain[0] : -INFINITY;
    }

    float domainMax() const
    {
        return m_trimmed ? m_domain[1] : INFINITY;
    }

private:
    T m_v[2] {};
    float m_t[2]{};    
    float m_slope {};

    float m_domain[2] {};
    bool m_trimmed {};
    enum { TwoPoints, Slope } m_mode;
};

template< class T, int MAX_SIZE >
class Polyline
{
public:
    Polyline()
    {
    }

    int size() const
    {
        return m_size;
    }

    void clear()
    {
        m_size = 0;
    }

    void pushSegment( const LineSegment<T>& seg )
    {
        if( m_size + 1 < MAX_SIZE )
        {
            m_segments[m_size++] = seg;
        }
    }

    bool evaluate( float t, T& val ) const
    {
        for( int i = 0; i < m_size; ++i )
        {
            auto& seg = m_segments[i];
            if( t >= seg.domainMin() && t < seg.domainMax() )
            {
                val = seg.evaluate( t );
                return true;
            }
        }
        return false;
    }

    const LineSegment<T>& operator[]( int i ) const
    {
        return m_segments[i];
    }

    float domainMin() const
    {
        return m_segments[0].domainMin();
    }

    float domainMax() const
    {
        return m_size ? m_segments[m_size - 1].domainMin() : 0.0f;
    }

private:
    LineSegment<T> m_segments[MAX_SIZE];
    int m_size {};
};

