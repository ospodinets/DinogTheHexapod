#include "Gait.h"
#include "Common.h"
#include "Arduino.h"

namespace
{
    const float waveGaitOnAsc = F_TOLERANCE;
    const float waveGaitOnDesc = 0.2f;

    const float rippleGaitOnAsc = 0.4f;
    const float rippleGaitOnDesc = 0.5f;

    const float tripodGaitOnAsc = 0.7f;
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

        SwitchingGait()
        {            
        }

        SwitchingGait* input( float velocity, float t )
        {
            return onInput( velocity, t );
        }

        virtual const char* name() const = 0;
      

    private:
        virtual SwitchingGait* onInput( float velocity, float t ) = 0;
    };

    class IdleGait : public SwitchingGait
    {
    public:
        IdleGait();
        const char* name() const override { return "Idle"; };

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
        const char* name() const override
        {
            return "Wave";
        };
    };

    class RippleGait : public SwitchingGait
    {
    public:
        RippleGait();

    private:
        float onEval( int legIndex, float t ) const override;
        SwitchingGait* onInput( float velocity, float t ) override;
        const char* name() const override
        {
            return "Switching";
        };
    };

    class TripodGait : public SwitchingGait
    {
    public:
        TripodGait();

    private:
        float onEval( int legIndex, float t ) const override;
        SwitchingGait* onInput( float velocity, float t ) override;
        const char* name() const override
        {
            return "Tripod";
        };
    }; 

    SwitchingGait* gait( SwitchingGait::Type type )
    {
        static SwitchingGait** s_gaits = nullptr;
        if( s_gaits )
        {
            s_gaits = new SwitchingGait*[SwitchingGait::Total];
            s_gaits[SwitchingGait::Idle] = new IdleGait;
            s_gaits[SwitchingGait::Wave] = new WaveGait;
            s_gaits[SwitchingGait::Ripple] = new RippleGait;
            s_gaits[SwitchingGait::Tripod] = new TripodGait;
        }
        return s_gaits[type];
    }

    SwitchingGait* s_currentGait = gait( SwitchingGait::Idle );
}

IdleGait::IdleGait()
    : SwitchingGait()
{
}

float IdleGait::onEval( int legIndex, float t ) const
{
    return 0.5f;
}

SwitchingGait* IdleGait::onInput( float velocity, float t )
{
    if( velocity > tripodGaitOnAsc )
        return gait( SwitchingGait::Tripod );

    if( velocity > rippleGaitOnAsc )
        return gait( SwitchingGait::Ripple );

    if( velocity > waveGaitOnAsc )
        return gait( SwitchingGait::Wave );

    return this;
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

float WaveGait::onEval( int legIndex, float t ) const
{
    return t < 1.0 ? -t : ( ( t - 1 ) / 5 );
}

SwitchingGait* WaveGait::onInput( float velocity, float t )
{
    if( velocity > tripodGaitOnAsc )
        return gait( SwitchingGait::Tripod );

    if( velocity > rippleGaitOnAsc )
        return gait( SwitchingGait::Ripple );

    if( velocity <= F_TOLERANCE )
        return gait( SwitchingGait::Idle );

    return this;
}

RippleGait::RippleGait()
    : SwitchingGait()
{
    m_period = 6.0;
    m_offsets[0] = 0.0f;
    m_offsets[1] = 2.0f;
    m_offsets[2] = 4.0f;
    m_offsets[3] = 1.0f;
    m_offsets[4] = 3.0f;
    m_offsets[5] = 5.0f;
}

float RippleGait::onEval( int legIndex, float t ) const
{
    return t < 2.0 ? ( -t / 2.0f ) : ( ( t - 2.0f ) / 4.0f );
}

SwitchingGait* RippleGait::onInput( float velocity, float t )
{
    if( velocity > tripodGaitOnAsc )
        return gait( SwitchingGait::Tripod );

    if( velocity < waveGaitOnDesc )
        return gait( SwitchingGait::Wave );

    if( velocity <= F_TOLERANCE )
        return gait( SwitchingGait::Idle );

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

float TripodGait::onEval( int legIndex, float t ) const
{
    return t < 1.0 ? -t : t - 1;
}

SwitchingGait* TripodGait::onInput( float velocity, float t )
{
    if( velocity < rippleGaitOnDesc )
        return gait( SwitchingGait::Ripple );

    if( velocity < waveGaitOnDesc )
        return gait( SwitchingGait::Wave );

    if( velocity <= F_TOLERANCE )
        return gait( SwitchingGait::Idle );

    return this;
}

Gait::Gait()
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
    auto next = s_currentGait->input( velocity, t );

    if( next != s_currentGait )
    {
        Serial.print( s_currentGait->name() );
        Serial.println( " gait finished" );
        
        s_currentGait = next;
        Serial.print( s_currentGait->name() );
        Serial.println( " gait started" );
    }

    return s_currentGait;
}