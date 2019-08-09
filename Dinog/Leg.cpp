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

    static const int coxaTrim = 93;
    static const int femurTrim = 100;
    static const int tibiaTrim = 3;

    static bool transaction = false;

    void evaluate( const Vec3f& pos, bool inverted, int& coxaValue, int& femurValue, int& tibiaValue )
    {
        auto Px_2 = pos[0] * pos[0];
        auto Py_2 = pos[1] * pos[1];
        auto Pz_2 = pos[2] * pos[2];

        auto P0 = sqrt( Px_2 + Py_2 );
        auto L4_2 = ( P0 - LegConfig::L1 ) * ( P0 - LegConfig::L1 ) + Pz_2;
        auto L4 = sqrt( L4_2 );

        coxaValue = degrees( atan( pos[1] / pos[0] ) ) + coxaTrim;
        float fFemurValue = acos( ( L2_2 + L4_2 - L3_2 ) / ( 2 * LegConfig::L2 * L4 ) ) + atan( pos[2] / ( P0 - LegConfig::L1 ) );
        if( inverted )
            fFemurValue = -fFemurValue;
        femurValue = degrees( fFemurValue ) + femurTrim;

        float fTibiaValue = acos( ( L4_2 - L2_2 - L3_2 ) / ( 2 * LegConfig::L2 * LegConfig::L3 ) );
        if( inverted )
            fTibiaValue = M_PI - fTibiaValue;

        tibiaValue = degrees( fTibiaValue ) + tibiaTrim;
    }
}

struct Leg::Impl
{
    bool inverted;

    ServoEx coxa;
    ServoEx femur;
    ServoEx tibia;

    int coxaValue = 90;
    int femurValue = 90;
    int tibiaValue = 90;

    // in Leg's coordinates
    Vec3f position;

    Impl( int coxaPin, int femurPin, int tibiaPin )
    {
        coxa.attach( coxaPin );
        femur.attach( femurPin );
        tibia.attach( tibiaPin );
    }

    ~Impl()
    {
        coxa.detach();
        femur.detach();
        tibia.detach();
    }    
};

Leg::Leg( int coxaPin, int femurPin, int tibiaPin, bool inverted )
    : m_impl( new Impl( coxaPin, femurPin, tibiaPin ) )
{
    m_impl->inverted = inverted;
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
        evaluate( m_impl->position, m_impl->inverted, 
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