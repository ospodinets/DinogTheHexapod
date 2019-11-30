#pragma once

#include "Common.h"

#include <Vec3f.h>
#include <ServoEx.h>

class Leg
{
public:
    Leg();
    ~Leg();

    void init( const LegConfig& config );

    void setPos( const Vec3f& value );
    const Vec3f& getPos() const;
    const Vec3f& getCenter();

    void center();

    // all setPos calls will be applied once commitTransaction is called
    static void startTransaction();
    static void commitTransaction( int msTransactionTime );

private:
    ServoEx m_coxa;
    ServoEx m_femur;
    ServoEx m_tibia;
    Vec3f m_position;
    const LegConfig* m_config;
};
