#include "Mover.h"
#include "Common.h"
#include "LegController.h"
#include "Gait.h"
#include "Arduino.h"

struct Mover::Impl
{
    LegController* legs[NUM_LEGS];
    ControlPose controlPose;
    float time {0};
};

Mover::Mover()
    : m_impl( new Impl )
{
    // initial setup
    Serial.println( "Initialize Mover" );
    startLegTransaction();
    for( int i = 0; i < NUM_LEGS; ++i )
    {
        m_impl->legs[i] = new LegController( getLegConfig( i ) );
    }
    commitLegTransaction( 1000 );

    //test
    ControlPose state;
    state.direction.set( 1, 0, 0 );

    setControlPose( state );
}

Mover::~Mover()
{
    Gait::release();
    for( int i = 0; i < NUM_LEGS; ++i )
    {
        delete m_impl->legs[i];
    }
    delete m_impl;
}

void Mover::setControlPose( const ControlPose & state )
{
    // For each leg:
    for( int i = 0; i < NUM_LEGS; ++i )
    {
        auto config = getLegConfig( i );
        // Solver: process input and evaluate Locomotion Vector
        // Set Locomotion Vector
        m_impl->legs[i]->setLocomotionVector( Vec3f { 0, 20, 0 } );
    }

    m_impl->controlPose = state;
}

void Mover::update( float dt )
{
    auto velocity = m_impl->controlPose.direction.length();

    // add multiplier to control gait speed
    m_impl->time += dt;

    auto gait = Gait::query( velocity, m_impl->time );

    // for each leg in the array
    for( int i = 0; i < NUM_LEGS; ++i )
    {
        auto ph = gait->evaluate( i, m_impl->time );
        m_impl->legs[i]->setPhaze( ph );
    }
}

