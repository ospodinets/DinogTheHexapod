#include "Common.h"
#include "Leg.h"
#include "Arduino.h"

LegConfig& getLegConfig( int index )
{
    static LegConfig s_config[] = {
        { 4, 3, 2, false, Vec3f{ 38, 67, 0 }, Quat( Vec3f::Z(), radians(-30) ) },
        { 50, 51, 52, true  },
        { 46, 47, 48, false },
        { 13, 12, 11, true  },
        { 10, 9, 8, false   },
        { 7, 6, 5, true     }
    };

    return s_config[index];
}

void startLegTransaction()
{
    Leg::startTransaction();
}

void commitLegTransaction( int milliseconds )
{
    Leg::commitTransaction( milliseconds );
}

float lerp( float v0, float v1, float t )
{
    return ( 1 - t ) * v0 + t * v1;
}