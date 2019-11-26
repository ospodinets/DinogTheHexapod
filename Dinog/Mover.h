#pragma once

#include "Common.h"
#include "LegController.h"

class Mover
{
public:
    Mover();
    ~Mover();

    void init();

    void setControlState( const ControlState& state );
    void update( float dt );

private:
    LegController m_legs[NUM_LEGS];
    ControlState m_controlState;
    float m_time { 0 };
};