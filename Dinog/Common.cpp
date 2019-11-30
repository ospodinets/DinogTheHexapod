#include "Common.h"
#include "Leg.h"
#include "Arduino.h"

LegConfig& getLegConfig( int index )
{
    static LegConfig s_config[] = {
        { 4, 3, 2, false, Vec3f{ 38, 67, 0 }, Quat( Vec3f::Z(), radians(-30) ), 93, 100, 3 },
        { 50, 51, 52, true, Vec3f { 0, 0, 0 }, Quat( Vec3f::Z(), radians( 0 ) ), 93, 100, 3 },
        { 46, 47, 48, false, Vec3f { 0, 0, 0 }, Quat( Vec3f::Z(), radians( 0 ) ), 93, 100, 3 },
        { 13, 12, 11, true, Vec3f { 0, 0, 0 }, Quat( Vec3f::Z(), radians( 0 ) ), 93, 100, 3 },
        { 10, 9, 8, false, Vec3f { 0, 0, 0 }, Quat( Vec3f::Z(), radians( 0 ) ), 93, 100, 3 },
        { 7, 6, 5, true, Vec3f { 0, 0, 0 }, Quat( Vec3f::Z(), radians( 0 ) ), 93, 100, 3 }
    };

    return s_config[index];
}

void loadConfig()
{

}

void saveConfig()
{

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