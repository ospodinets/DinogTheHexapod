#include "Mover.h"
#include "Arduino.h"
#include "Gait.h"

Mover::Mover()
{
    
}

Mover::~Mover()
{
    Gait::release();    
}

void Mover::init()
{
    // initial setup
    Serial.println( "Initialize Mover" );
    startLegTransaction();
    for( int i = 0; i < NUM_LEGS; ++i )
    {
        m_legs[i].init( getLegConfig( i ) );
    }
    commitLegTransaction( 1000 );
}

void Mover::setControlState( const ControlState& state )
{
    // For each leg:
    for( int i = 0; i < NUM_LEGS; ++i )
    {
        // auto config = getLegConfig( i );
        // Solver: process input and evaluate Locomotion Vectors for each leg
        // Set Locomotion Vectors to leg controllers
        //m_legs[i].setLocomotionVector( Vec3f { 0, i < 3 ? 20 : -20, 0 } );
    }

    m_controlState = state;
}

void Mover::update( float dt )
{
    auto velocity = m_controlState.direction.length();

    // add multiplier to control gait speed
    m_time += dt / 2;

    auto gait = Gait::query( velocity, m_time );

    // for each leg in the array
    for( int i = 0; i < NUM_LEGS; ++i )
    {
        auto ph = gait->evaluate( i, m_time );
        m_legs[i].setPhaze( ph );
    }
}

