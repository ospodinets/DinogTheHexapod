#pragma once

#include <math.h>
#include <Vec3.h>

struct ControlPose
{
    Vec3f direction { 0.0f, 0.0f, 0.0f };
    float torque { 0.0f };
    Vec3f normal { 0.0f, 0.0f, 1.0f };    
};

#define F_TOLERANCE 1e-03

