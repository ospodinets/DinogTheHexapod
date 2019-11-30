#pragma once

#include "Common.h"
#include "LegController.h"

class Mover
{
public:
    Mover();
    ~Mover();

    void init();

    void setControl( const Control& control );
    void update( float dt );

private:
    LegController m_legs[NUM_LEGS];
    Control m_control;
    float m_time { 0 };
};