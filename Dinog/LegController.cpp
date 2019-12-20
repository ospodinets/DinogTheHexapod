#include "LegController.h"
#include "Common.h"
#include "Arduino.h"

#include <MathUtils.h>

namespace
{

    void printM( const Mat4x4& mat )
    {
        for( int i = 0; i < 4; ++i )
        {
            Serial.print( "|" );
            for( int j = 0; j < 4; ++j )
            {
                Serial.print( mat[i][j] );
                if( j != 3 )
                    Serial.print( ", " );
            }
            Serial.println( "|" );
        }
    }

    void printQ( const Quat& q )
    {
        Serial.print( "Q[" );
        for( int i = 0; i < 4; ++i )
        {   
            Serial.print( q[i] );  
            if( i != 3 )
                Serial.print( ", " );
        }
        Serial.println( "]" );
    }

    static const float S_Z_SWING_ELEVATION = 40.0f;
    static const float SMOOTH_FACTOR = 4;

    Vec3f evaluateSwing( float phaze, const Vec3f& p0, const Vec3f& p1 )
    {
        phaze = fabs( phaze );
        auto midZ = max( p1[2], p0[2] ) + S_Z_SWING_ELEVATION;

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
    , m_p {}
    , m_pTmp {}
    , m_rot { 1.0f }
{}

LegController::~LegController()
{
}

void LegController::init( const LegConfig& legConfig )
{   
    m_rot = legConfig.rotation.toMatrix3x3();
    m_rot.inverse();

    m_pT0 = m_pT1 = m_pTmp = m_p = m_p0 = m_p1 = m_leg.getCenter();
    m_leg.init( legConfig );  
    m_stance = true;
}

void LegController::setInput( const Vec3f& locomotionVector, float elevation )
{
    auto VlocLen2 = locomotionVector.length2();
    if( fabs( VlocLen2 ) < F_TOLERANCE )
    {
        m_pT0 = m_pT1 = m_leg.getCenter();
    }
    else
    {
        auto Vloc = m_rot.mult( locomotionVector );
        auto Pc = m_leg.getCenter();
        Pc[2] -= elevation; 
        auto VlocHalf = Vloc * 0.5;
        m_pT0 = Pc + VlocHalf;
        m_pT1 = Pc - VlocHalf;
    }
}

void LegController::evaluate( float phaze )
{ 
    // update end points
    m_p0 = m_p0 + ( m_pT0 - m_p0 ) / SMOOTH_FACTOR;
    m_p1 = m_p1 + ( m_pT1 - m_p1 ) / SMOOTH_FACTOR;

    bool stance = phaze >= 0;

    if( m_stance != stance )
    {
        // phaze switched
        // if swing is activated, current leg position is used as p1 
        // so, we can turn on swing at any time

        if( !stance )
        {
            m_pTmp = m_p;
        }
        else
        {
            m_pTmp = m_p1;
        }
        m_stance = stance;
    }    

    m_p = stance ? evaluateStance( phaze, m_p0, m_p1 ) :
        evaluateSwing( phaze, m_p0, m_pTmp );    
    m_leg.setPos( m_p );
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