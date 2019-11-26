#include "Gait.h"
#include "Common.h"
#include "Arduino.h"
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

        virtual const char* name() = 0;

    private:
        virtual SwitchingGait* onInput( float velocity, float t ) = 0;
    };

    class IdleGait : public SwitchingGait
    {
    public:
        IdleGait();
        ~IdleGait();
        const char* name() override { return "Idle"; };

    private:
        float onEval( int legIndex, float t ) const override;
        SwitchingGait* onInput( float velocity, float t ) override;
    };

    class WaveGait : public SwitchingGait
    {
    public:
        WaveGait();
        ~WaveGait();
        const char* name() override { return "Wave"; };

    private:
        float onEval( int legIndex, float t ) const override;
        SwitchingGait* onInput( float velocity, float t ) override;
    };

    class TripodGait : public SwitchingGait
    {
    public:
        TripodGait();
        ~TripodGait();
        const char* name() override
        {
            return "Tripod";
        };

    private:
        float onEval( int legIndex, float t ) const override;
        SwitchingGait* onInput( float velocity, float t ) override;
    };

    class TransitionGait : public SwitchingGait
    {
    public:
        TransitionGait( SwitchingGait * currentGait, SwitchingGait * targetGait, float t );
        ~TransitionGait();
        const char* name() override { return "Transition"; };

    private:
        float onEval( int legIndex, float t ) const override;
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
        float m_lastT {};
    };

    SwitchingGait* s_currentGait { nullptr };
}



IdleGait::IdleGait()
    : SwitchingGait()
{
}

IdleGait::~IdleGait()
{
}
    
float IdleGait::onEval( int legIndex, float t ) const
{
    return 0.5f;
}

SwitchingGait* IdleGait::onInput( float velocity, float t )
{
    if( velocity < F_TOLERANCE )
        return this;

    if( velocity > 0 && velocity < 5 )
        return new TransitionGait( this, new WaveGait, t );

    return new TransitionGait( this, new TripodGait, t );
}


WaveGait::WaveGait()
    : SwitchingGait()
{
    m_period = 6.0;
    m_offsets[0] = 0.0f;
    m_offsets[1] = 1.0f;
    m_offsets[2] = 2.0f;
    m_offsets[3] = 3.0f;
    m_offsets[4] = 4.0f;
    m_offsets[5] = 5.0f;
}

WaveGait::~WaveGait()
{
}


float WaveGait::onEval( int legIndex, float t ) const
{
    return t < 1.0 ? -t : ( ( t - 1 ) / 5 );
}

SwitchingGait* WaveGait::onInput( float velocity, float t ) 
{
    if( fabs( velocity ) < F_TOLERANCE )
        return new TransitionGait( this, new IdleGait, t );

    return this;
}

TripodGait::TripodGait()
    : SwitchingGait()
{
    m_period = 2.0;
    m_offsets[0] = 0.0f;
    m_offsets[1] = 1.0f;
    m_offsets[2] = 0.0f;
    m_offsets[3] = 1.0f;
    m_offsets[4] = 0.0f;
    m_offsets[5] = 1.0f;
}

TripodGait::~TripodGait()
{
}


float TripodGait::onEval( int legIndex, float t ) const
{
    return t < 1.0 ? -t : t - 1;
}

SwitchingGait* TripodGait::onInput( float velocity, float t )
{
    if( fabs( velocity ) > 7 )
        return this;    

    return new TransitionGait( this, new WaveGait, t );
}

TransitionGait::TransitionGait( SwitchingGait * currentGait, SwitchingGait * targetGait, float t )
    : SwitchingGait()
    , m_target { targetGait }
    , m_lastT { t }
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


float TransitionGait::onEval( int legIndex, float t ) const
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

    auto dt = t - m_lastT;

    m_progress += dt;
    if( m_progress > s_T )
    {
        m_progress = s_T;
    }

    return this;
}

Gait::Gait()
{
}

Gait::~Gait()
{    
}

float Gait::period() const
{
    return m_period;
}

bool Gait::isPeriodic() const
{
    return fabs( m_period ) > F_TOLERANCE;
}

float Gait::evaluate( int legIndex, float t ) const
{
    double intP;
    auto ts = m_offsets[legIndex] + t;
    auto fractT = modf( ts / m_period, &intP );

    return onEval( legIndex, fractT * m_period );
}

const Gait* const Gait::query( float velocity, float t )
{    
    if( !s_currentGait )
    {
        s_currentGait = new IdleGait();
        Serial.println( "Idle gait started" );
    }        

    auto next = s_currentGait->input( velocity, t );

    if( next != s_currentGait )
    {
        Serial.print( s_currentGait->name() );
        Serial.println( " gait finished" );
        delete s_currentGait;
        s_currentGait = next;
        Serial.print( s_currentGait->name() );
        Serial.println( " gait started" );
    }

    return s_currentGait;
}

void Gait::release()
{
    if( s_currentGait )
    {
        delete s_currentGait;
        s_currentGait = nullptr;
    }        
}
