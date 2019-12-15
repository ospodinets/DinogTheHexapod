#include "Gait.h"
#include "Common.h"
#include "Arduino.h"

#include <LineSegment.h>

namespace
{
    using Segment = LineSegment<float>;
    using Curve = Polyline<float, 3>;

    const float mixerPeriod = 1.0f;
    const float mixerIdlePeriod = 0.2f;

    const float waveGaitOnAsc = F_TOLERANCE;
    const float waveGaitOnDesc = 0.2f;

    const float rippleGaitOnAsc = 0.4f;
    const float rippleGaitOnDesc = 0.8f;

    const float tripodGaitOnAsc = 0.95f;
    const float tripodGaitOnDesc = 1.0f;

    class SwitchingGait : public Gait
    {
    public:
        enum Type
        {
            Idle, 
            Wave, 
            Ripple, 
            Tripod,

            Total
        };

        SwitchingGait( float stanceSlope )
            : m_stanceSlope { stanceSlope }
        {            
        }

        SwitchingGait* input( float velocity, float t )
        {
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

    private:
        virtual SwitchingGait* onInput( float velocity, float t ) = 0;

        float m_stanceSlope {};
    };

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
        void setup( SwitchingGait* from, SwitchingGait* to, float t, float period = mixerPeriod );

    private:
        float onEval( int legIndex, float t ) const override;
        SwitchingGait* onInput( float velocity, float t ) override;

        float m_t {};
        Curve m_curves[NUM_LEGS];
        SwitchingGait* m_to {};
    };

    class IdleGait : public SwitchingGait
    {
    public:
        IdleGait();
#ifdef DEBUG_TRACE
        const char* name() const override { return "Idle"; }
#endif 

    private:
        float onEval( int legIndex, float t ) const override;
        SwitchingGait* onInput( float velocity, float t ) override;
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
            s_gaits[SwitchingGait::Idle] = new IdleGait;
            s_gaits[SwitchingGait::Wave] = new WaveGait;
            s_gaits[SwitchingGait::Ripple] = new RippleGait;
            s_gaits[SwitchingGait::Tripod] = new TripodGait;
        }
        return s_gaits[type];
    }

    SwitchingGait* mix( SwitchingGait* from, SwitchingGait* to, float t, float T )
    {
#ifdef DEBUG_TRACE
        Serial.print( "Mixing " );
        Serial.print( from->name() );
        Serial.print( " with " );
        Serial.println( to->name() );

#endif // DEBUG_TRACE

        static GaitMixer s_mixer;
        s_mixer.setup( from, to, t, T );
        return &s_mixer;
    }


    SwitchingGait* s_currentGait = gait( SwitchingGait::Idle );
}

GaitMixer::GaitMixer()
    : SwitchingGait( 0.0f )
{
}

void GaitMixer::setup( SwitchingGait* from, SwitchingGait* to, float t, float period )
{
    m_t = t;
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
    if( t > m_t + m_period )
        return m_to;

    return this;
}

IdleGait::IdleGait()
    : SwitchingGait( 0.0f )
{
}

float IdleGait::onEval( int legIndex, float t ) const
{
    return 0.5f;
}

SwitchingGait* IdleGait::onInput( float velocity, float t )
{
    if( velocity > tripodGaitOnAsc )
        return mix( this, gait( SwitchingGait::Tripod ), t, mixerPeriod );

    if( velocity > rippleGaitOnAsc )
        return mix( this, gait( SwitchingGait::Ripple ), t, mixerPeriod );

    if( velocity > waveGaitOnAsc )
        return mix( this, gait( SwitchingGait::Wave ), t, mixerPeriod );

    return this;
}


WaveGait::WaveGait()
    : SwitchingGait( 0.2f )
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
    if( velocity > tripodGaitOnAsc )
        return mix( this, gait( SwitchingGait::Tripod ), t, mixerPeriod );

    if( velocity > rippleGaitOnAsc )
        return mix( this, gait( SwitchingGait::Ripple ), t, mixerPeriod );

    if( velocity <= F_TOLERANCE )
        return mix( this, gait( SwitchingGait::Idle ), t, mixerIdlePeriod );

    return this;
}

RippleGait::RippleGait()
    : SwitchingGait( 0.25f )
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
    if( velocity > tripodGaitOnAsc )
        return mix( this, gait( SwitchingGait::Tripod ), t, mixerPeriod );

    if( velocity < waveGaitOnDesc )
        return mix( this, gait( SwitchingGait::Wave ), t, mixerPeriod );

    if( velocity <= F_TOLERANCE )
        return mix( this, gait( SwitchingGait::Idle ), t, mixerIdlePeriod );

    return this;
}

TripodGait::TripodGait()
    : SwitchingGait( 1.0f )
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
    if( velocity < rippleGaitOnDesc )
        return mix( this, gait( SwitchingGait::Ripple ), t, mixerPeriod );

    if( velocity < waveGaitOnDesc )
        return mix( this, gait( SwitchingGait::Wave ), t, mixerPeriod );

    if( velocity <= F_TOLERANCE )
        return mix( this, gait( SwitchingGait::Idle ), t, mixerIdlePeriod );

    return this;
}

Gait::Gait()
{
}

float Gait::evaluate( int legIndex, float t ) const
{
    auto ph = onEval( legIndex, t );

#ifdef DEBUG_TRACE
    Serial.print( "Gait::evaluate(" );
    Serial.print( t );
    Serial.print( ")=" );
    Serial.println( ph );
#endif

    return ph;
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