#include "LegController.h"
#include "Common.h"
#include "Leg.h"
#include "Arduino.h"
#include "Mat4x4.h"

namespace
{
    static const float S_MAXZ = 70.0f;

    Vec3f evaluateSwing( float phaze, const Vec3f& p0, const Vec3f& p1 )
    {
        phaze = fabs( phaze );
        auto midZ = ( p1[2] + p0[2] ) / 2 + S_MAXZ;

        return Vec3f { lerp( p1[0], p0[0], phaze ),
            lerp( p1[1], p0[1], phaze ),
            phaze < 0.5 ? lerp( p1[2], midZ, phaze ) : lerp( midZ, p0[2], phaze ) };
    }

    Vec3f evaluateStance( float phaze, const Vec3f& p0, const Vec3f& p1 )
    {
        return Vec3f { lerp( p0[0], p1[0], phaze ),
                      lerp( p0[1], p1[1], phaze ),
                      lerp( p0[2], p1[2], phaze ) };
    }    
}

struct LegController::Impl
{
    Leg* leg {};

    Vec3f p0, p1;
    float phaze {};
    float lastPhaze {};

    Mat4x4 transform;
};

LegController::LegController( const LegConfig & config )
    : m_impl( new Impl )
{
    Serial.print( "Create leg: " );

    Serial.print( "Coxa: " );
    Serial.print( config.coxaPin, DEC );
    Serial.print( " Femur: " );
    Serial.print( config.femurPin, DEC );
    Serial.print( " Tibia: " );
    Serial.print( config.tibiaPin, DEC );
    Serial.print( "\n" );
    
    m_impl->transform.set( 1.0 );
    m_impl->transform.mult( Mat4x4::translationMatrix( config.offset ) );
    m_impl->transform.mult( config.rotation.toMatrix() );

    m_impl->leg = new Leg( config );
    m_impl->leg->center();
}

LegController::~LegController()
{
    delete m_impl;
}

void LegController::setLocomotionVector( const Vec3f & val )
{
    // calculate p0, p1

    // TEMP
    //m_impl->p0.set( LegConfig::L1 + LegConfig::L2 - 10, -val[1], -LegConfig::L3 );
    //m_impl->p1.set( LegConfig::L1 + LegConfig::L2 - 10, val[1], -LegConfig::L3 );
    //m_impl->p1 = m_impl->p0 = m_impl->leg->getCenter();
}

void LegController::setPhaze( float phaze )
{
    /*
    if( fabs( phaze - m_impl->lastPhaze ) > F_TOLERANCE )
    {
        bool stance = false;
        if( fabs( phaze ) < F_TOLERANCE )
        {
            stance = m_impl->lastPhaze < 0;
        }
        else
        {
            stance = phaze > 0;
        }

        auto pos = stance ? evaluateStance( phaze, m_impl->p0, m_impl->p1 ) :
            evaluateSwing( phaze, m_impl->p0, m_impl->p1 );

        m_impl->leg->setPos( pos );

        m_impl->lastPhaze = phaze;
    }    
    */
}
