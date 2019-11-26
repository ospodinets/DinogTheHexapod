#pragma once

#include "Leg.h"

#include <Vec3f.h>
#include <Mat4x4.h>

struct LegConfig;

class LegController
{
public:
    LegController();
    ~LegController();

    void init( const LegConfig& legConfig );

    void setLocomotionVector( const Vec3f& val );
    void setPhaze( float phaze );

private:
    Leg m_leg;
    Vec3f m_p0, m_p1;
    float m_phaze;
    float m_lastPhaze;
    Mat4x4 m_transform;
};
