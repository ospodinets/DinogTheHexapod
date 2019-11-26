#include "Leg.h"

#include <Arduino.h>

namespace
{
    static const int L2_2 = LegConfig::L2 * LegConfig::L2;
    static const int L3_2 = LegConfig::L3 * LegConfig::L3;

    Vec3f CENTER { LegConfig::L1 + LegConfig::L2,
                   0,
                   - LegConfig::L3 };

    bool transaction = false;

    void evaluate( const Vec3f& pos, const LegConfig& legConfig, int& coxaValue, int& femurValue, int& tibiaValue )
    {
        auto Px_2 = pos[0] * pos[0];
        auto Py_2 = pos[1] * pos[1];
        auto Pz_2 = pos[2] * pos[2];

        auto P0 = sqrt( Px_2 + Py_2 );
        auto L4_2 = ( P0 - LegConfig::L1 ) * ( P0 - LegConfig::L1 ) + Pz_2;
        auto L4 = sqrt( L4_2 );

        coxaValue = degrees( atan( pos[1] / pos[0] ) ) + legConfig.coxaTrim;
        float fFemurValue = acos( ( L2_2 + L4_2 - L3_2 ) / ( 2 * LegConfig::L2 * L4 ) ) + atan( pos[2] / ( P0 - LegConfig::L1 ) );
        if( legConfig.inverted )
            fFemurValue = -fFemurValue;
        femurValue = degrees( fFemurValue ) + legConfig.femurTrim;

        float fTibiaValue = acos( ( L4_2 - L2_2 - L3_2 ) / ( 2 * LegConfig::L2 * LegConfig::L3 ) );
        if( legConfig.inverted )
            fTibiaValue = M_PI - fTibiaValue;

        tibiaValue = degrees( fTibiaValue ) + legConfig.tibiaTrim;
    }
}

Leg::Leg( )
    : m_config( nullptr )
{  
    m_position = CENTER;
}

Leg::~Leg()
{
}

void Leg::init( const LegConfig& config )
{
    m_config = &config;

    Serial.print( "Init leg: " );

    Serial.print( "Coxa: " );
    Serial.print( m_config->coxaPin, DEC );
    Serial.print( " Femur: " );
    Serial.print( m_config->femurPin, DEC );
    Serial.print( " Tibia: " );
    Serial.print( m_config->tibiaPin, DEC );
    Serial.print( "\n" );

    m_coxa.attach( m_config->coxaPin );
    m_femur.attach( m_config->femurPin );
    m_tibia.attach( m_config->tibiaPin );
}

void Leg::setPos( const Vec3f & value )
{
    if( !m_position.equal( value, F_TOLERANCE ) )
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

Vec3f & Leg::getCenter()
{
    return CENTER;
}

void Leg::center()
{
    setPos( CENTER );
}

void Leg::startTransaction()
{
    ServoGroupMove.start();
}

void Leg::commitTransaction( int msTransactionTime )
{
    ServoGroupMove.commit( msTransactionTime );
}