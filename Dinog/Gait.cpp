#include "Gait.h"
#include "Common.h"
#include <math.h>

namespace
{
    class IdleGait;
    class WaveGait;
    class TransitionGait;

    class SwitchingGait : public Gait
    {
    public:
        SwitchingGait()
        {
        }

        SwitchingGait* input( float velocity, float t )
        {
            return onInput( velocity, t );
        }

    private:
        virtual SwitchingGait* onInput( float velocity, float t ) = 0;
    };

    class IdleGait : public SwitchingGait
    {
    public:
        IdleGait();
        ~IdleGait();

    private:
        float onEval( int legIndex, float t ) override;
        SwitchingGait* onInput( float velocity, float t ) override;
    };

    class WaveGait : public SwitchingGait
    {
    public:
        WaveGait();
        ~WaveGait();

    private:
        float onEval( int legIndex, float t ) override;
        SwitchingGait* onInput( float velocity, float t ) override;
    };

    class TransitionGait : public SwitchingGait
    {
    public:
        TransitionGait( SwitchingGait * currentGait, SwitchingGait * targetGait, float t );
        ~TransitionGait();

    private:
        float onEval( int legIndex, float t );
        SwitchingGait* onInput( float velocity, float t ) override;

    private:

        const float s_T = 1.0f;

        struct
        {
            float p0;
            float p1;
        } m_legs[NUM_LEGS];
        float m_progress { 0.0f };
        SwitchingGait* m_target { nullptr };
    };
}

struct Gait::Impl
{
    float period { 1.0f };
    float legPhazeShifts[NUM_LEGS] {};

    static SwitchingGait* s_currentGait;
};

SwitchingGait* Gait::Impl::s_currentGait { nullptr };

IdleGait::IdleGait()
    : SwitchingGait()
{
}

IdleGait::~IdleGait()
{
}
    
float IdleGait::onEval( int legIndex, float t )
{
    return 0.5f;
}

SwitchingGait* IdleGait::onInput( float velocity, float t )
{
    if( fabs( velocity ) < F_TOLERANCE )
        return this;

    return new TransitionGait( this, new WaveGait, t );
}


WaveGait::WaveGait()
    : SwitchingGait()
{
    m_impl->period = 6.0;
    m_impl->legPhazeShifts[0] = 0.0f;
    m_impl->legPhazeShifts[1] = 1.0f;
    m_impl->legPhazeShifts[2] = 2.0f;
    m_impl->legPhazeShifts[3] = 3.0f;
    m_impl->legPhazeShifts[4] = 4.0f;
    m_impl->legPhazeShifts[5] = 5.0f;
}

WaveGait::~WaveGait()
{
}


float WaveGait::onEval( int legIndex, float t ) 
{
    auto ts = m_impl->legPhazeShifts[legIndex] + t;
    return t < 1.0 ? -t : ( ( t - 1 ) / 5 );
}

SwitchingGait* WaveGait::onInput( float velocity, float t ) 
{
    if( fabs( velocity ) > F_TOLERANCE )
        return this;

    return new TransitionGait( this, new IdleGait, t );
}

TransitionGait::TransitionGait( SwitchingGait * currentGait, SwitchingGait * targetGait, float t )
    : SwitchingGait()
    , m_target { targetGait }
{
    for( int i = 0; i < NUM_LEGS; ++i )
    {
        m_legs[i].p0 = currentGait->evaluate( i, t );
        m_legs[i].p1 = targetGait->evaluate( i, t );
    }
}

TransitionGait::~TransitionGait()
{
}


float TransitionGait::onEval( int legIndex, float t )
{
    const auto& l = m_legs[legIndex];

    if( fabs( m_progress - s_T ) < F_TOLERANCE || fabs( l.p0 - l.p1 ) < F_TOLERANCE )
    {
        return l.p1;
    }

    auto T = s_T;
    float p1 = l.p1;
    // linear transition from phaze A to B:
    float t0 = ( l.p0 * T ) / ( l.p0 - l.p1 );
    // changes phase            
    if( t0 >= 0 && t0 <= T )
    {
        p1 = l.p0 >= 0 ? 1.0f : 0.0f;
        T = t0;
    }
    return l.p0 + t * ( ( p1 - l.p0 ) / T );
}

SwitchingGait* TransitionGait::onInput( float velocity, float t )
{
    if( fabs( m_progress - s_T ) < F_TOLERANCE )
        return m_target;

    m_progress += t;
    if( m_progress > s_T )
    {
        m_progress = s_T;
    }

    return this;
}

Gait::Gait()
    : m_impl( new Impl() )
{
}

Gait::~Gait()
{
    delete m_impl;
}

float Gait::period() const
{
    return m_impl->period;
}

bool Gait::isPeriodic() const
{
    return fabs( m_impl->period ) > F_TOLERANCE;
}

float Gait::evaluate( int legIndex, float t )
{
    double intP;
    auto fractT = modf( t / m_impl->period, &intP );

    return onEval( legIndex, fractT * m_impl->period );
}

Gait* Gait::query( float velocity, float t )
{    
    if( !Impl::s_currentGait )
        Impl::s_currentGait = new IdleGait();

    auto next = Impl::s_currentGait->input( velocity, t );

    if( next != Impl::s_currentGait )
    {
        delete Impl::s_currentGait;
        Impl::s_currentGait = next;
    }

    return Impl::s_currentGait;
}
