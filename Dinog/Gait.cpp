#include "Gait.h"
#include "Common.h"
#include "Arduino.h"

#include <LineSegment.h>

#define USE_GAIT_MIXER 1

namespace
{
    using Segment = LineSegment<float>;
    using Curve = Polyline<float, 3>;

    const float waveGaitOnAsc = F_TOLERANCE;
    const float waveGaitOnDesc = 0.2f;

    const float rippleGaitOnAsc = 0.4f;
    const float rippleGaitOnDesc = 0.8f;

    const float tripodGaitOnAsc = 0.95f;
    const float tripodGaitOnDesc = 1.0f;

    // Do not allow switching gait frequently
    const float minGaitUptime = 2.0f;

    class SwitchingGait : public Gait
    {
    public:
        enum Type
        {
            Idle, 
            Wave, 
            Ripple, 
            Tripod,
            Mixer,

            Total
        };

        SwitchingGait( Type type, float stanceSlope )
            : m_type { type }
            , m_stanceSlope { stanceSlope }
        {
            if ( fabs(m_stanceSlope) > F_TOLERANCE )
                m_speedMultiplier = 1 / m_stanceSlope;
        }

        Type type() const
        {
            return m_type;
        }

        SwitchingGait* input( float velocity, float t )
        {
            m_uptime = t - m_t;
            return onInput( velocity, t );
        }

        float getStanceSlope() const
        {
            return m_stanceSlope;
        }

#ifdef DEBUG_TRACE
        virtual const char* name() const = 0;
#endif   
        float clamp( int leg, float t ) const
        {
            double intP {};
            return modf( (t + m_offsets[leg]) / m_period, &intP ) * m_period;
        }

        void start( float t )
        {
            m_t = t;
            m_uptime = 0.0f;
        }

        float uptime() const
        {
            return m_uptime;
        }

    private:
        virtual SwitchingGait* onInput( float velocity, float t ) = 0;

        float m_stanceSlope {};
        Type m_type;

        float m_t {};
        float m_uptime {};
    };

#if USE_GAIT_MIXER
    class GaitMixer : public SwitchingGait
    {
    public:
        GaitMixer();
#ifdef DEBUG_TRACE
        const char* name() const override
        {
            return "Gait mixer";
        }
#endif 
        void setup( SwitchingGait* from, SwitchingGait* to, float t, float period = minGaitUptime );

    private:
        float onEval( int legIndex, float t ) const override;
        SwitchingGait* onInput( float velocity, float t ) override;

        Curve m_curves[NUM_LEGS];
        SwitchingGait* m_to {};
    };
#endif
    
    class IdleGait : public SwitchingGait
    {
    public:
        IdleGait();
#ifdef DEBUG_TRACE
        const char* name() const override { return "Idle"; }
#endif 
        void setup( SwitchingGait* prev, float t );

    private:
        float onEval( int legIndex, float t ) const override;
        SwitchingGait* onInput( float velocity, float t ) override;

        float m_phazes[NUM_LEGS] {};
    };

    class WaveGait : public SwitchingGait
    {
    public:
        WaveGait();        

    private:
        float onEval( int legIndex, float t ) const override;
        SwitchingGait* onInput( float velocity, float t ) override;

#ifdef DEBUG_TRACE
        const char* name() const override { return "Wave"; }
#endif 
    };

    class RippleGait : public SwitchingGait
    {
    public:
        RippleGait();

    private:
        float onEval( int legIndex, float t ) const override;
        SwitchingGait* onInput( float velocity, float t ) override;

#ifdef DEBUG_TRACE
        const char* name() const override { return "RippleGait"; }
#endif 
    };

    class TripodGait : public SwitchingGait
    {
    public:
        TripodGait();

    private:
        float onEval( int legIndex, float t ) const override;
        SwitchingGait* onInput( float velocity, float t ) override;
#ifdef DEBUG_TRACE
        const char* name() const override { return "Tripod"; }
#endif 
    };     

    SwitchingGait* gait( SwitchingGait::Type type )
    {
        static SwitchingGait** s_gaits = nullptr;
        if( !s_gaits )
        {
            s_gaits = new SwitchingGait*[SwitchingGait::Total];
            s_gaits[SwitchingGait::Idle] = nullptr;
            s_gaits[SwitchingGait::Wave] = new WaveGait;
            s_gaits[SwitchingGait::Ripple] = new RippleGait;
            s_gaits[SwitchingGait::Tripod] = new TripodGait;
            s_gaits[SwitchingGait::Mixer] = nullptr;
        }
        return s_gaits[type];
    }

    SwitchingGait* idle( SwitchingGait* prev, float t )
    {
        static IdleGait s_idle;
        s_idle.setup( prev, t );
        return &s_idle;
    }

    SwitchingGait* mix( SwitchingGait* from, SwitchingGait* to, float t )
    {
#if USE_GAIT_MIXER
#ifdef DEBUG_TRACE
        Serial.print( "Mixing " );
        Serial.print( from->name() );
        Serial.print( " with " );
        Serial.println( to ? to->name() : "Idle");

#endif // DEBUG_TRACE

        if( to )
        {
            static GaitMixer s_mixer;
            s_mixer.setup( from, to, t, minGaitUptime );
            return &s_mixer;            
        }
        
        return idle( from, t );
#else
        return to ? to : idle( from, t );
#endif
    }    

    SwitchingGait* s_currentGait = idle( nullptr, 0.0 );
}

#if USE_GAIT_MIXER
GaitMixer::GaitMixer()
    : SwitchingGait( Type::Mixer, 1.0f )
{
}

void GaitMixer::setup( SwitchingGait* from, SwitchingGait* to, float t, float period )
{
    m_to = to;
    m_period = period;    

    auto t0 = t;
    auto t1 = t + m_period;

#ifdef DEBUG_TRACE
    Serial.print( "Mixing gaits: " );
    Serial.print( " t0=" );
    Serial.print( t0 );
    Serial.print( ", t1=" );
    Serial.println( t1 );
#endif

    // setup mixing curves for each leg
    for( int i = 0; i < NUM_LEGS; ++i )
    {
        auto ph0 = from->evaluate( i, t0 );
        auto ph1 = to->evaluate( i, t1 );
        auto slope = to->getStanceSlope();
        auto& curve = m_curves[i];
        curve.clear();

#ifdef DEBUG_TRACE
        Serial.print( " ph0=" );
        Serial.print( ph0 );
        Serial.print( ", ph1=" );
        Serial.print( ph1 );
        Serial.print( " | " );
#endif

        Segment stance;
        float tmid = 0.0f, phmid = 0.0f;

        if( ph1 >= 0 )
        {
#ifdef DEBUG_TRACE
            Serial.print( "1." );
#endif
            stance.set( slope, ph1, t1 );
            
            // special case
            if( fabs( ph0 - stance.evaluate( t0 ) ) < F_TOLERANCE )
            {
#ifdef DEBUG_TRACE
                Serial.print( "sp." );
#endif
                tmid = t0;
            }
            else
            {
#ifdef DEBUG_TRACE
                Serial.print( "1." );
#endif
                if( stance.findT( 0, tmid ) && tmid >= t0 && tmid <= t1 )
                {
#ifdef DEBUG_TRACE
                    Serial.print( "1." );
#endif
                    if( fabs( t0 - tmid) > F_TOLERANCE )
                        curve.pushSegment( Segment( ph0 >= 0.0f ? -F_TOLERANCE : ph0, -1.0f, t0, tmid ) );
                }
                else
                {
#ifdef DEBUG_TRACE
                    Serial.print( "2." );
#endif
                    tmid = t0 + ( t1 - t0 ) / 3;
                    phmid = stance.evaluate( tmid );
                    curve.pushSegment( Segment( ph0 >= 0.0f ? -F_TOLERANCE : ph0, -(1 - phmid), t0, tmid ) );
                }
            }

            if( fabs( tmid - t1 ) > F_TOLERANCE )
            {
                stance.setTrim( tmid, t1 );
                curve.pushSegment( stance );
            }
        }
        else
        {
#ifdef DEBUG_TRACE
            Serial.print( "2." );
#endif
            if( ph0 > 0 )
            {
#ifdef DEBUG_TRACE
                Serial.print( "1." );
#endif
                stance.set( slope, ph0, t0 );
                if( stance.findT( 1.0, tmid ) && tmid >= t0 && tmid <= t1 )
                {
#ifdef DEBUG_TRACE
                    Serial.print( "1." );
#endif
                    curve.pushSegment( Segment( ph0, 1.0, t0, tmid ) );
                }
                else
                {
#ifdef DEBUG_TRACE
                    Serial.print( "2." );
#endif
                    tmid = t0 + 2 * ( t1 - t0 ) / 3;
                    phmid = stance.evaluate( tmid );
                    curve.pushSegment( Segment( ph0, phmid, t0, tmid ) );
                }

                curve.pushSegment( Segment( -F_TOLERANCE, ph1, tmid, t1 ) );
            }
            else
            {
#ifdef DEBUG_TRACE
                Serial.print( "2." );
#endif
                curve.pushSegment( Segment( ph0, ph1, t0, t1 ) );
            }            
        }


#ifdef DEBUG_TRACE
        for( int j = 0; j < curve.size(); ++j )
        {
            auto& seg = curve[j];
            Serial.print( "| p(" );
            Serial.print( seg.domainMin() );
            Serial.print( ")=" );
            Serial.print( seg.evaluate( seg.domainMin() ) );
            Serial.print( ": p(" );
            Serial.print( seg.domainMax() );
            Serial.print( ")=" );
            Serial.print( seg.evaluate( seg.domainMax() ) );
        }
        Serial.println();
#endif
    }
   
}

float GaitMixer::onEval( int legIndex, float t ) const
{
    auto& curve = m_curves[legIndex];

    float phaze = 0;
    curve.evaluate( t, phaze );

    return phaze;
}

SwitchingGait* GaitMixer::onInput( float velocity, float t )
{
    if( uptime() > m_period )
        return m_to;

    return this;
}

#endif

IdleGait::IdleGait()
    : SwitchingGait( Type::Idle, 0.0f )
{
}

void IdleGait::setup( SwitchingGait* prev, float t )
{
    for( int i = 0; i < NUM_LEGS; ++i )
    {
        float ph = 0.5f;
        if( prev )
        {
            ph = prev->evaluate( i, t );
            if( ph < 0 )
                ph = 1 - ph;
        }
        m_phazes[i] = ph;
    }
}

float IdleGait::onEval( int legIndex, float t ) const
{
    return m_phazes[legIndex];
}

SwitchingGait* IdleGait::onInput( float velocity, float t )
{
    if( velocity > tripodGaitOnAsc )
        return mix( this, gait( SwitchingGait::Tripod ), t );

    if( velocity > rippleGaitOnAsc )
        return mix( this, gait( SwitchingGait::Ripple ), t );

    if( velocity > waveGaitOnAsc )
        return mix( this, gait( SwitchingGait::Wave ), t );

    return this;
}


WaveGait::WaveGait()
    : SwitchingGait( Type::Wave, 0.2f )
{
    m_period = 6.0;
    m_offsets[0] = 0.0f;
    m_offsets[1] = 1.0f;
    m_offsets[2] = 2.0f;
    m_offsets[3] = 3.0f;
    m_offsets[4] = 4.0f;
    m_offsets[5] = 5.0f;
}

float WaveGait::onEval( int legIndex, float t ) const
{
    t = clamp( legIndex, t );
    return t < 1.0f ? -t : ( ( t - 1.0f ) / 5.0f );
}

SwitchingGait* WaveGait::onInput( float velocity, float t )
{
    if( uptime() > minGaitUptime )
    {
        if( velocity > tripodGaitOnAsc )
            return mix( this, gait( SwitchingGait::Tripod ), t );

        if( velocity > rippleGaitOnAsc )
            return mix( this, gait( SwitchingGait::Ripple ), t );
    }

    if( velocity <= F_TOLERANCE )
        return mix( this, nullptr, t );

    return this;
}

RippleGait::RippleGait()
    : SwitchingGait( Type::Ripple, 0.25f )
{
    m_period = 6.0f;
    m_offsets[0] = 0.0f;
    m_offsets[1] = 2.0f;
    m_offsets[2] = 4.0f;
    m_offsets[3] = 1.0f;
    m_offsets[4] = 3.0f;
    m_offsets[5] = 5.0f;
}

float RippleGait::onEval( int legIndex, float t ) const
{
    t = clamp( legIndex, t );
    return t < 2.0f ? ( -t / 2.0f ) : ( ( t - 2.0f ) / 4.0f );
}

SwitchingGait* RippleGait::onInput( float velocity, float t )
{
    if( uptime() > minGaitUptime )
    {
        if( velocity > tripodGaitOnAsc )
            return mix( this, gait( SwitchingGait::Tripod ), t );

        if( velocity < waveGaitOnDesc )
            return mix( this, gait( SwitchingGait::Wave ), t );
    }

    if( velocity <= F_TOLERANCE )
        return mix( this, nullptr, t );

    return this;
}

TripodGait::TripodGait()
    : SwitchingGait( Type::Tripod, 1.0f )
{
    m_period = 2.0f;
    m_offsets[0] = 0.0f;
    m_offsets[1] = 1.0f;
    m_offsets[2] = 0.0f;
    m_offsets[3] = 1.0f;
    m_offsets[4] = 0.0f;
    m_offsets[5] = 1.0f;
}

float TripodGait::onEval( int legIndex, float t ) const
{
    t = clamp( legIndex, t );
    return t < 1.0f ? -t : t - 1.0f;
}

SwitchingGait* TripodGait::onInput( float velocity, float t )
{
    if( uptime() > minGaitUptime )
    {
        if( velocity < rippleGaitOnDesc )
            return mix( this, gait( SwitchingGait::Ripple ), t );

        if( velocity < waveGaitOnDesc )
            return mix( this, gait( SwitchingGait::Wave ), t );
    }

    if( velocity <= F_TOLERANCE )
        return mix( this, nullptr, t );

    return this;
}

Gait::Gait()
{
}

float Gait::evaluate( int legIndex, float t ) const
{
    return onEval( legIndex, t );
}

const Gait* const Gait::query( float velocity, float t )
{
    auto next = s_currentGait->input( velocity, t );

    if( next != s_currentGait )
    {
#ifdef DEBUG_TRACE
        Serial.print( s_currentGait->name() );
        Serial.println( " gait finished" );
        Serial.print( next->name() );
        Serial.println( " gait started" );
#endif
        s_currentGait = next;        
    }

    return s_currentGait;
}