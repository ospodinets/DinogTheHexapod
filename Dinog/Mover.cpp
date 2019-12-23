#include "Mover.h"
#include "Arduino.h"
#include "Gait.h"

namespace
{
    const float SPEED_MULTIPLIER = 5.0f;
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

    m_control = control;
    m_solver.setControl( control );
}

void Mover::update( float dt )
{
    if( m_locomotionEnabled )
    {
        auto velocity = m_solver.getVelocity();
        auto gait = Gait::query( velocity, m_time );
                
        m_time += gait->getSpeedMultiplier() * SPEED_MULTIPLIER * velocity * dt;

        Vec3f locomotionVector {};
        float elevation {};

        // for each leg in the array
        for( int i = 0; i < NUM_LEGS; ++i )
        {
            m_solver.evaluate( i, locomotionVector, elevation );
            m_legs[i].setInput( locomotionVector, elevation );

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
