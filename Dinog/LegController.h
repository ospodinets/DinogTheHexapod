#pragma once

#include "Leg.h"

#include <Vec3f.h>
#include <Mat3x3.h>

class LegController
{
public:
    LegController();
    ~LegController();

    void init( const Leg::Config& legConfig );

    void setInput( const Vec3f& locomotionVector, float elevation );
    void evaluate( float phaze );

    // used only if locomotion is disabled
    void moveToPos( const Vec3f& pos );
    void centerLeg();

private:
    Leg m_leg;
    Vec3f m_p0, m_p1, m_p, m_pTmp;
    Vec3f m_pT0, m_pT1;
    Mat3x3 m_rot;

    bool m_stance;
};
