#pragma once

#include <math.h>
#include "Vec3f.h"
#include "Quat.h"

static const float F_TOLERANCE = 1e-03;
static const int NUM_LEGS = 6;

struct Control
{   
    float elevation { 0.0f };
    float torque { 0.0f };
    float forward { 0.0f };
    float right { 0.0f };
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

