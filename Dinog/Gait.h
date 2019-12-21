#pragma once

#include "Common.h"

class Gait
{
public:

    // t = [0; T]
    // phase = f(t) 0 -> 1 or -1
    // phase < 0 - swing 
    // phase > 0 - stance
    float evaluate( int legIndex, float t ) const;
    float getSpeedMultiplier() const
    {
        return m_speedMultiplier;
    }

    // Gait state machine
    static const Gait* const query( float velocity, float t );   

protected:
    Gait();

private:   
    virtual float onEval( int legIndex, float t ) const = 0;    

protected:
    float m_period { 1.0f };
    float m_offsets[NUM_LEGS] {};
    float m_speedMultiplier {1.0f};
};
