#include "Leg.h"
#include "Common.h"
#include "ServoEx.h"
#include "Arduino.h"

namespace
{
    static const int L2_2 = LegConfig::L2 * LegConfig::L2;
    static const int L3_2 = LegConfig::L3 * LegConfig::L3;

    Vec3f CENTER { LegConfig::L1 + LegConfig::L2,
                   0,
                   - LegConfig::L3 };

    static bool transaction = false;

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

struct Leg::Impl
{
    ServoEx coxa;
    ServoEx femur;
    ServoEx tibia;

    int coxaValue = 90;
    int femurValue = 90;
    int tibiaValue = 90;

    // in Leg's coordinates
    Vec3f position;

    const LegConfig& legConfig;

    Impl( const LegConfig& legConfig_ )
        : legConfig( legConfig_ )
    {
        coxa.attach( legConfig.coxaPin );
        femur.attach( legConfig.femurPin );
        tibia.attach( legConfig.tibiaPin );
    }

    ~Impl()
    {
        coxa.detach();
        femur.detach();
        tibia.detach();
    }    
};

Leg::Leg( const LegConfig& config )
    : m_impl( new Impl( config ) )
{   
}

Leg::~Leg()
{
    delete m_impl;
}


void Leg::setPos( const Vec3f & value )
{
    if( !m_impl->position.equal( value, F_TOLERANCE ) )
    {
        m_impl->position = value;
        evaluate( m_impl->position, m_impl->legConfig, 
            m_impl->coxaValue, m_impl->femurValue, m_impl->tibiaValue );

        m_impl->coxa.write( m_impl->coxaValue );
        m_impl->femur.write( m_impl->femurValue );
        m_impl->tibia.write( m_impl->tibiaValue );
    }
}

const Vec3f & Leg::getPos() const
{
    return m_impl->position;
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