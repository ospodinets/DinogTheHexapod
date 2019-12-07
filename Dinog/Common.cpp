#include "Common.h"
#include "Leg.h"
#include "Arduino.h"
#include <EEPROM.h>

namespace 
{
    const int legsAddr = 0;
}


LegConfig& getLegConfig( int index )
{
    static LegConfig s_config[] = {
        /*0*/{ 4, 3, 2, false, Vec3f{ 38, 67, 0 }, Quat( Vec3f::Z(), radians(-30) ), 93, 100, 3 },
        /*1*/ { 50, 51, 52, true, Vec3f { 0, 0, 0 }, Quat( Vec3f::Z(), radians( 0 ) ), 93, 100, 3 },
        /*2*/{ 46, 47, 48, true, Vec3f { 0, 0, 0 }, Quat( Vec3f::Z(), radians( 0 ) ), 93, 100, 3 },
        /*3*/{ 13, 12, 11, false, Vec3f { 0, 0, 0 }, Quat( Vec3f::Z(), radians( 0 ) ), 93, 100, 3 },
        /*4*/{ 10, 9, 8, false, Vec3f { 0, 0, 0 }, Quat( Vec3f::Z(), radians( 0 ) ), 93, 100, 3 },
        /*5*/{ 7, 6, 5, true, Vec3f { 0, 0, 0 }, Quat( Vec3f::Z(), radians( 0 ) ), 93, 100, 3 }
    };

    return s_config[index];
}

void loadConfig()
{
    Serial.println( "Load Trims: " );
    int addr = 0;
    for( int i = 0; i < NUM_LEGS; i++ )
    {
        auto& cfg = getLegConfig( i );

        cfg.coxaTrim = ( int8_t )EEPROM.read( addr++ );
        cfg.femurTrim = ( int8_t ) EEPROM.read( addr++ );
        cfg.tibiaTrim = ( int8_t ) EEPROM.read( addr++ );

        Serial.print( "Leg: " );
        Serial.print( i );
        Serial.print( " " );
        Serial.print( cfg.coxaTrim );
        Serial.print( " " );
        Serial.print( cfg.femurTrim );
        Serial.print( " " );
        Serial.println( cfg.tibiaTrim );
    }
}

void saveConfig()
{
    Serial.println( "Saving Trims: " );
    int addr = 0;
    for( int i = 0; i < NUM_LEGS; i++ )
    {
        auto& cfg = getLegConfig( i );

        EEPROM.update( addr++, cfg.coxaTrim );
        EEPROM.update( addr++, cfg.femurTrim );
        EEPROM.update( addr++, cfg.tibiaTrim );

        Serial.print( "Leg: " );
        Serial.print( i );
        Serial.print( " " );
        Serial.print( cfg.coxaTrim );
        Serial.print( " " );
        Serial.print( cfg.femurTrim );
        Serial.print( " " );
        Serial.println( cfg.tibiaTrim );
    }
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

float map_f( long x, long in_min, long in_max, float out_min, float out_max )
{
    return float( x - in_min ) * ( out_max - out_min ) / float( in_max - in_min ) + out_min;
}