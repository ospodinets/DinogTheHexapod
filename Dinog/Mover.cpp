#include "Mover.h"
#include "Arduino.h"
#include "Gait.h"

namespace
{
    const float SPEED_MULTIPLIER = 6.0f;
}


Mover::Mover()
{    
}

void Mover::init()
{
    // initial setup
    Serial.println( "Initialize Mover" );
    for( int i = 0; i < NUM_LEGS; ++i )
    {
        m_legs[i].init( getLegConfig( i ) );
    }
}

void Mover::setControl( const Control& control )
{
    if( !m_locomotionEnabled )
        return;

    // For each leg:
    for( int i = 0; i < NUM_LEGS; ++i )
    {
        // auto config = getLegConfig( i );
        // Solver: process input and evaluate Locomotion Vectors for each leg
        // Set Locomotion Vectors to leg controllers
        m_legs[i].setLocomotionVector( Vec3f { 0, i < 3 ? -20 : 20, 0 } );
    }

    m_control = control;
}

void Mover::update( float dt )
{
    if( m_locomotionEnabled )
    {
        auto velocity = m_control.direction.length();

        // add multiplier to control gait speed
        // auto timeshift = SPEED_MULTIPLIER * velocity;
        // if( timeshift < 1.0f )
        //     timeshift = 1.0f;
        // m_time += timeshift * dt;
        m_time += dt / 3;

        auto gait = Gait::query( velocity, m_time );

        // for each leg in the array
        for( int i = 0; i < NUM_LEGS; ++i )
        {
            auto ph = gait->evaluate( i, m_time );
            m_legs[i].evaluate( ph );
        }
    }
}

void Mover::enableLocomotion( bool enable )
{
    m_locomotionEnabled = enable;
    for( int i = 0; i < NUM_LEGS; ++i )
    {
        centerLeg( i );
    }
}

void Mover::evaluateLeg( int leg, const Vec3f& pos )
{
    if( !m_locomotionEnabled )
    {
        m_legs[leg].moveToPos( pos );
    }
}

void Mover::centerLeg( int leg )
{
    m_legs[leg].centerLeg();
}
