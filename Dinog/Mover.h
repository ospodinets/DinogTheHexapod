#pragma once

#include "Common.h"
#include "LegController.h"

class Mover
{
public:
    Mover();

    void init();

    void setControl( const Control& control );
    void update( float dt );

    void enableLocomotion( bool enable );
    void evaluateLeg( int leg, const Vec3f& pos );
    void centerLeg( int leg );

private:
    LegController m_legs[NUM_LEGS];
    Control m_control;
    float m_time { 0 };
    bool m_locomotionEnabled { false };
};