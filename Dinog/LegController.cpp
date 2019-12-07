#include "LegController.h"
#include "Common.h"
#include "Arduino.h"

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


LegController::LegController()
    : m_p0 { }
    , m_p1 { }
    , m_phaze { 0.5 }
    , m_lastPhaze { 0.5 }
    , m_transform { 1.0f }    
{}

LegController::~LegController()
{
}

void LegController::init( const LegConfig& legConfig )
{
    m_transform.mult( Mat4x4::translationMatrix( legConfig.offset ) );
    m_transform.mult( legConfig.rotation.toMatrix() );

    m_p0 = m_p1 = m_leg.getCenter();
    m_leg.init( legConfig );    
}

void LegController::setLocomotionVector( const Vec3f & val )
{
    // calculate p0, p1

    // TEMP
    //m_p0.set( LegConfig::L1 + LegConfig::L2 - 10, -val[1], -LegConfig::L3 );
    //m_p1.set( LegConfig::L1 + LegConfig::L2 - 10, val[1], -LegConfig::L3 );
    m_p1 = m_p0 = m_leg.getCenter();
}

void LegController::evaluate( float phaze )
{
    if( fabs( phaze - m_lastPhaze ) > F_TOLERANCE )
    {
        bool stance = false;
        if( fabs( phaze ) < F_TOLERANCE )
        {
            stance = m_lastPhaze < 0;
        }
        else
        {
            stance = phaze > 0;
        }

        auto pos = stance ? evaluateStance( phaze, m_p0, m_p1 ) :
            evaluateSwing( phaze, m_p0, m_p1 );

        m_leg.setPos( pos );

        m_lastPhaze = phaze;
    }
}

void LegController::moveToPos( const Vec3f& pos )
{
    Vec3f target;

    auto center = m_leg.getCenter();
    target[0] = center[0] + pos[0];
    target[1] = center[1] + pos[1];
    target[2] = center[2];

    m_leg.setPos( target );
}

void LegController::centerLeg()
{
    m_leg.setPos( m_leg.getCenter(), true );
}
