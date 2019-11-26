#pragma once

#include "Common.h"

class Gait
{
public:        
    virtual ~Gait();

    // T
    float period() const; 
    bool isPeriodic() const;

    // t = [0; T]
    // phase = f(t) 0 -> 1 or -1
    // phase < 0 - swing 
    // phase > 0 - stance

    float evaluate( int legIndex, float t ) const;

    // Gait state machine
    static const Gait* const query( float velocity, float t );
    static void release();

protected:
    Gait();

private:   
    virtual float onEval( int legIndex, float t ) const = 0;    

protected:
    float m_period { 1.0f };
    float m_offsets[NUM_LEGS] {};    
};
