#pragma once

#include "Common.h"
#include <Vec3f.h>

struct Control;

class Solver
{
public:
    Solver();

    void setControl( const Control& control );
    void evaluate( int legIndex, Vec3f& locomotionVector, float& elevation );

private:
    Vec3f m_V;
    float m_T;
    float m_H;

    Vec3f m_Rs[NUM_LEGS] {};
};
