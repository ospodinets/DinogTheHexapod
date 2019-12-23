#pragma once

#include "Common.h"
#include <Vec3f.h>

class Solver
{
public:
    Solver();

    void setControl( const Control& control );
    float getVelocity() const;
    void evaluate( int legIndex, Vec3f& locomotionVector, float& elevation );

private:
    Vec3f m_direction;
    float m_torque;
    float m_elevation;
    Vec3f m_tangents[NUM_LEGS] {};
};
