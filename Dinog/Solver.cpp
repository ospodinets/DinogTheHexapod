#include "Solver.h"
#include "Leg.h"

#include <Arduino.h>
#include <MathUtils.h>

namespace
{
    const float MAX_ABS_ELEVATION = 40.0f;
    const float MAX_ABS_LOCOMOTION = 60.0f;

    float evaluateElevation( const Vec3f& offset, const Vec3f& N, float baseH )
    {
        return baseH - ( N[0] * offset[0] + N[1] * offset[1] ) / N[2];
    }
}

Solver::Solver()
{
    // tangent vectors for each leg
    for( int i = 0; i < NUM_LEGS; ++i )
    {
        auto R = Leg::getConfig( i ).offset;
        R.normalize();
        m_tangents[i] = R.cross( - Vec3f::Z() );
    }
}

void Solver::setControl( const Control & control )
{
    m_torque = control.torque;
    m_elevation = control.elevation;  
    m_direction.set( -control.forward,
                     -control.right, 0.0f );    
}

float Solver::getVelocity() const
{
    return max( fabs(m_direction[0]), max( fabs(m_direction[1]), fabs(m_torque) ) );
}

void Solver::evaluate( int legIndex, Vec3f& locomotionVector, float& elevation )
{
    const auto& lc = Leg::getConfig( legIndex );

    if( fabs( m_torque ) >= F_TOLERANCE )
    {  
        auto Vt = m_tangents[legIndex] * m_torque * MAX_ABS_LOCOMOTION;
        locomotionVector = m_direction * MAX_ABS_LOCOMOTION + Vt;
    }
    else
    {
        locomotionVector = m_direction * MAX_ABS_LOCOMOTION;
    }

    if( locomotionVector.length2() > MAX_ABS_LOCOMOTION * MAX_ABS_LOCOMOTION )
    {
        locomotionVector.normalize();
        locomotionVector *= MAX_ABS_LOCOMOTION;
    }

    elevation = evaluateElevation( lc.offset, Vec3f::Z(), m_elevation * MAX_ABS_ELEVATION );
}