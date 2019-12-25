#include "Mover.h"
#include "Gait.h"

#include <Arduino.h>

namespace
{
    const float SPEED_MULTIPLIER = 5.0f;
}

Mover::Mover()
{    
}

void Mover::init()
{
#ifdef DEBUG_TRACE
    Serial.println( "Initialize Mover" );
#endif
    for( int i = 0; i < NUM_LEGS; ++i )
    {
        m_legs[i].init( Leg::getConfig( i ) );
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
        
        auto gaitTimeGradient = gait->getSpeedMultiplier() * SPEED_MULTIPLIER * dt;
        auto velTimeGradient = velocity < 0.1 ? 0.1f : velocity;
                
        m_time += velTimeGradient * velTimeGradient;

        Vec3f locomotionVector {};
        float elevation {};

        for( int i = 0; i < NUM_LEGS; ++i )
        {
            m_solver.evaluate( i, locomotionVector, elevation );
            auto ph = gait->evaluate( i, m_time );            
            m_legs[i].setInput( locomotionVector, elevation, ph );
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
