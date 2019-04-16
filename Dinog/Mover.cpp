#include "Mover.h"
#include "Common.h"
#include "Leg.h"
#include "Gait.h"
#include "Arduino.h"

namespace
{    
    struct LegState
    {
        Leg* leg;
    };

}

struct Mover::Impl
{
    LegState legs[NUM_LEGS];
    ControlPose controlPose;
    float time {0};
};

Mover::Mover()
    : m_impl( new Impl )
{
    m_impl->legs[0].leg = new Leg( 4, 3, 2, false );
    m_impl->legs[1].leg = new Leg( 50, 51, 52, true );
    m_impl->legs[2].leg = new Leg( 46, 47, 48, false );
    m_impl->legs[3].leg = new Leg( 13, 12, 11, true );
    m_impl->legs[4].leg = new Leg( 10, 9, 8, false );
    m_impl->legs[5].leg = new Leg( 7, 6, 5, true );  
}

Mover::~Mover()
{
    delete m_impl;
}

void Mover::setControlPose( const ControlPose & state )
{
    // Solver: process input

    // Gait factory find gait
    // if new gait not equal to the current one
    // switch gait 
}

void Mover::update( float dt )
{
    // tests
#if 0
    static float delay = 0.0f;        

    delay += dt;

    if( delay > 2 )
    {
        delay = 0;

        static int state = 0;

        Leg::startTransaction();

        switch( state )
        {
            case 0:
                for( int i = 0; i < NUM_LEGS; ++i )
                    m_impl->legs[i].leg->setPos( Vec3f( 74, 0, -55 ) );
                state = 1;
                break;
            
            case 1:
                for( int i = 0; i < NUM_LEGS; ++i )
                    m_impl->legs[i].leg->setPos( Vec3f( 62, 0, -120 ) );
                state = 2;
                break;    

            case 2:
                state = 0;
                for( int i = 0; i < NUM_LEGS; ++i )
                    m_impl->legs[i].leg->setPos( Vec3f( 83, 0, -80 ) );
                break;

            default:
                break;
        }

        Leg::commitTransaction( 1000 );
    }
#endif



    auto velocity = m_impl->controlPose.direction.length();

    // add multiplier to control gait speed
    m_impl->time += dt;

    auto gait = Gait::query( velocity, m_impl->time );

    // for each leg in the array
    //  gait->evaluate
    //  update leg
}

