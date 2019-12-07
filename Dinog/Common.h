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
void loadConfig();
void saveConfig();

void startLegTransaction();
void commitLegTransaction( int milliseconds );

float lerp( float v0, float v1, float t );
float map_f( long x, long in_min, long in_max, float out_min, float out_max );


#define F_TOLERANCE 1e-03
#define NUM_LEGS 6

