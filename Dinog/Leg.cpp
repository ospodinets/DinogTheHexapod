#include "Leg.h"
#include "Common.h"

#include <MathUtils.h>
#include <Arduino.h>
#include <EEPROM.h>

namespace
{
    static const int EEPROM_trimsOffset = 0;
    static const int L2_2 = Leg::Config::L2 * Leg::Config::L2;
    static const int L3_2 = Leg::Config::L3 * Leg::Config::L3;

    Vec3f CENTER { Leg::Config::L1 + Leg::Config::L2,
                   0,
                   - Leg::Config::L3 + 10 };

    Vec3f HOME { Leg::Config::L1 + Leg::Config::L2,
                 0,
                 -Leg::Config::L3 + 50 };

    bool transaction = false;

    void evaluate( const Vec3f& pos, const Leg::Config& legConfig, int& coxaValue, int& femurValue, int& tibiaValue )
    {
        auto Px_2 = pos[0] * pos[0];
        auto Py_2 = pos[1] * pos[1];
        auto Pz_2 = pos[2] * pos[2];

        auto P0 = sqrt( Px_2 + Py_2 );
        auto L4_2 = ( P0 - Leg::Config::L1 ) * ( P0 - Leg::Config::L1 ) + Pz_2;
        auto L4 = sqrt( L4_2 );

        coxaValue = degrees( atan( -pos[1] / pos[0] ) ) + legConfig.coxaTrim;
        float fFemurValue = acos( ( L2_2 + L4_2 - L3_2 ) / ( 2 * Leg::Config::L2 * L4 ) ) + atan( pos[2] / ( P0 - Leg::Config::L1 ) );
        if( legConfig.inverted )
            fFemurValue = -fFemurValue;
        femurValue = degrees( fFemurValue ) + legConfig.femurTrim;

        float fTibiaValue = acos( ( L4_2 - L2_2 - L3_2 ) / ( 2 * Leg::Config::L2 * Leg::Config::L3 ) );
        if( legConfig.inverted )
            fTibiaValue = M_PI - fTibiaValue;

        tibiaValue = degrees( fTibiaValue ) + legConfig.tibiaTrim;
    }
}


Leg::Config& Leg::getConfig( int index )
{
    static Config s_config[] = {
        /*0*/ { 4, 3, 2, false,    Vec3f { 63.3, 37.8, 0 },  Quat( Vec3f::Z(), radians( 45 ) ),   93, 100, 3 },
        /*1*/ { 50, 51, 52, true,  Vec3f { 0, 63, 0 },       Quat( Vec3f::Z(), radians( 90 ) ),   93, 100, 3 },
        /*2*/ { 46, 47, 48, true,  Vec3f { -63.3, 37.8, 0 }, Quat( Vec3f::Z(), radians( 135 ) ),  93, 100, 3 },
        /*3*/ { 13, 12, 11, false, Vec3f { -63.3, -37.8, 0 },Quat( Vec3f::Z(), radians( -135 ) ), 93, 100, 3 },
        /*4*/ { 10, 9, 8, false,   Vec3f { 0, -63, 0 },      Quat( Vec3f::Z(), radians( -90 ) ),  93, 100, 3 },
        /*5*/ { 7, 6, 5, true,     Vec3f { 63.3, -37.8, 0 }, Quat( Vec3f::Z(), radians( -45 ) ),  93, 100, 3 }
    };

    return s_config[index];
}

void Leg::loadConfig()
{
#ifdef DEBUG_TRACE
    Serial.println( "Load Trims: " );
#endif
    int addr = 0;
    for( int i = 0; i < NUM_LEGS; i++ )
    {
        auto& cfg = getConfig( i );

        cfg.coxaTrim = ( int8_t ) EEPROM.read( addr++ );
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

void Leg::saveConfig()
{
#ifdef DEBUG_TRACE
    Serial.println( "Saving Trims: " );
#endif
    int addr = EEPROM_trimsOffset;
    for( int i = 0; i < NUM_LEGS; i++ )
    {
        auto& cfg = getConfig( i );

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


Leg::Leg( )
    : m_config( nullptr )
{   
}

Leg::~Leg()
{
}

void Leg::init( const Leg::Config& config )
{
    m_config = &config;

    setPos( getHome(), true );

    m_coxa.attach( m_config->coxaPin );
    m_femur.attach( m_config->femurPin );
    m_tibia.attach( m_config->tibiaPin );
}

void Leg::setPos( const Vec3f & value, bool force = false )
{
    if( !m_position.equal( value, F_TOLERANCE ) || force )
    {
        int coxa, femur, tibia;
        m_position = value;
        evaluate( m_position, *m_config, coxa, femur, tibia );

        m_coxa.write( coxa );
        m_femur.write( femur );
        m_tibia.write( tibia );
    }
}

const Vec3f & Leg::getPos() const
{
    return m_position;
}

const Vec3f & Leg::getCenter() const
{
    return CENTER;
}

const Vec3f & Leg::getHome() const
{
    return HOME;
}
