#include "Solver.h"
#include "Common.h"
#include "Arduino.h"

namespace
{
    const float MAX_ABS_ELEVATION = 65.0f;
    const float MAX_ABS_LOCOMOTION = 60.0f;

    float evaluateElevation( const Vec3f& offset, const Vec3f& N, float baseH )
    {
        return baseH - ( N[0] * offset[0] + N[1] * offset[1] ) / N[2];
    }
}

Solver::Solver()
{
    for( int i = 0; i < NUM_LEGS; ++i )
    {
        m_Rs[i] = getLegConfig( i ).offset;
        m_Rs[i].normalize();
    }
}

void Solver::setControl( const Control & control )
{
    m_T = control.torque;
    m_H = control.height;

    //auto len = control.dir.length2();
    //if( fabs( len ) >= F_TOLERANCE )
    if( fabs( control.thr ) >= F_TOLERANCE )
    {
        //m_V = -control.dir * MAX_ABS_LOCOMOTION;
        m_V.set( MAX_ABS_LOCOMOTION, 0.0f, 0.0f );
    }
    else
    {
        m_V.set( 0.0f, 0.0f, 0.0f );
    }
}

void Solver::evaluate( int legIndex, Vec3f& locomotionVector, float& elevation )
{
    const auto& lc = getLegConfig( legIndex );

    if( fabs( m_T ) >= F_TOLERANCE )
    {        
        auto Vt = m_Rs[legIndex].cross( Vec3f( 0, 0, -1 ) );
        Vt *= 2 * m_T * MAX_ABS_LOCOMOTION;
        locomotionVector = m_V + Vt;
    }
    else
    {
        locomotionVector = m_V;
    }

    elevation = evaluateElevation( lc.offset, Vec3f( 0, 0, 1 ), m_H * MAX_ABS_ELEVATION );
}