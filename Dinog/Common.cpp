#include "Common.h"
#include "Leg.h"
#include "Arduino.h"
#include "Controller.h"
#include <EEPROM.h>

namespace
{
    int s_EEPROMtrims = 0;
    int s_EEPROMLimits = 20;
}

LegConfig& getLegConfig( int index )
{
    static LegConfig s_config[] = {
        /*0*/{ 4, 3, 2, false,    Vec3f { 63.3, 37.8, 0 },  Quat( Vec3f::Z(), radians( 45 ) ),   93, 100, 3 },
        /*1*/{ 50, 51, 52, true,  Vec3f { 0, 63, 0 },       Quat( Vec3f::Z(), radians( 90 ) ),   93, 100, 3 },
        /*2*/{ 46, 47, 48, true,  Vec3f { -63.3, 37.8, 0 }, Quat( Vec3f::Z(), radians( 135 ) ),  93, 100, 3 },
        /*3*/{ 13, 12, 11, false, Vec3f { -63.3, -37.8, 0 },Quat( Vec3f::Z(), radians( -135 ) ), 93, 100, 3 },
        /*4*/{ 10, 9, 8, false,   Vec3f { 0, -63, 0 },      Quat( Vec3f::Z(), radians( -90 ) ),  93, 100, 3 },
        /*5*/{ 7, 6, 5, true,     Vec3f { 63.3, -37.8, 0 }, Quat( Vec3f::Z(), radians( -45 ) ),  93, 100, 3 }
    };

    return s_config[index];
}

void loadConfig()
{
#ifdef DEBUG_TRACE
    Serial.println( "Load Trims: " );
#endif
    int addr = 0;
    for( int i = 0; i < NUM_LEGS; i++ )
    {
        auto& cfg = getLegConfig( i );

        cfg.coxaTrim = ( int8_t )EEPROM.read( addr++ );
        cfg.femurTrim = ( int8_t ) EEPROM.read( addr++ );
        cfg.tibiaTrim = ( int8_t ) EEPROM.read( addr++ );
#ifdef DEBUG_TRACE
        Serial.print( "Leg: " );
        Serial.print( i );
        Serial.print( " " );
        Serial.print( cfg.coxaTrim );
        Serial.print( " " );
        Serial.print( cfg.femurTrim );
        Serial.print( " " );
        Serial.println( cfg.tibiaTrim );
#endif
    }
}

void saveConfig()
{
#ifdef DEBUG_TRACE
    Serial.println( "Saving Trims: " );
#endif
    int addr = s_EEPROMtrims;
    for( int i = 0; i < NUM_LEGS; i++ )
    {
        auto& cfg = getLegConfig( i );

        EEPROM.update( addr++, cfg.coxaTrim );
        EEPROM.update( addr++, cfg.femurTrim );
        EEPROM.update( addr++, cfg.tibiaTrim );
#ifdef DEBUG_TRACE
        Serial.print( "Leg: " );
        Serial.print( i );
        Serial.print( " " );
        Serial.print( cfg.coxaTrim );
        Serial.print( " " );
        Serial.print( cfg.femurTrim );
        Serial.print( " " );
        Serial.println( cfg.tibiaTrim );
#endif
    }
}

void loadChannelLimits( int * limits )
{
#ifdef DEBUG_TRACE
    Serial.println( "Load Limits: " );
#endif
    int addr = s_EEPROMLimits;    
    for( int i = 0; i < Controller::NUM_CHANNELS; i++ )
    {
        int min {};
        EEPROM.get( addr, min );
        addr += sizeof( min );
        int max {};
        EEPROM.get( addr, max );
        addr += sizeof( max );

        ( *limits++ ) = min;
        ( *limits++ ) = max;
        
#ifdef DEBUG_TRACE
        Serial.print( "Channel: " );
        Serial.print( i );
        Serial.print( " " );
        Serial.print( min );
        Serial.print( " " );
        Serial.println( max );
#endif
    }
}

void saveChannelLimits( int * limits )
{
#ifdef DEBUG_TRACE
    Serial.println( "Save Limits: " );
#endif
    int addr = s_EEPROMLimits;
    for( int i = 0; i < Controller::NUM_CHANNELS; i++ )
    {
        auto min = ( *limits++ );
        auto max = ( *limits++ );
        EEPROM.put( addr, min );
        addr += sizeof( min );
        EEPROM.put( addr, max );
        addr += sizeof( max );

#ifdef DEBUG_TRACE
        Serial.print( "Channel: " );
        Serial.print( i );
        Serial.print( " " );
        Serial.print( min );
        Serial.print( " " );
        Serial.println( max );
#endif
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