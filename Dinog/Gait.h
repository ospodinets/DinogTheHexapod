#pragma once

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

    float evaluate( int legIndex, float t );

    // Gait state machine
    static Gait* query( float velocity, float t );
    static void release();

protected:
    Gait();

private:   
    virtual float onEval( int legIndex, float t ) = 0;    

protected:
    struct Impl;
    Impl* m_impl;
};
