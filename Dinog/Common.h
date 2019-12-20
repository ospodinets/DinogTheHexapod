#pragma once

#include <math.h>
#include "Vec3f.h"
#include "Quat.h"

static const float F_TOLERANCE = 1e-03;
static const int NUM_LEGS = 6;

struct Control
{    
    Vec3f dir { 0.0f, 0.0f, 0.0f };
    float torque { 0.0f };
    float thr { 0.0f };
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

