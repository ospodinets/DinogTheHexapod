#pragma once

#include <math.h>
#include "Vec3f.h"
#include "Quat.h"

struct Control
{    
    Vec3f direction { 0.0f, 0.0f, 0.0f };
    float torque { 0.0f };
    Vec3f normal { 0.0f, 0.0f, 1.0f };
    float height { 0.0f };
};


struct LegConfig
{
    static const int L1 = 28;
    static const int L2 = 55;
    static const int L3 = 80;

    int coxaPin;
    int femurPin; 
    int tibiaPin;
    bool inverted;
    Vec3f offset;
    Quat rotation;

    int coxaTrim;
    int femurTrim;
    int tibiaTrim;
};

LegConfig& getLegConfig( int index );
void startLegTransaction();
void commitLegTransaction( int milliseconds );

float lerp( float v0, float v1, float t );




#define F_TOLERANCE 1e-03
#define NUM_LEGS 6

