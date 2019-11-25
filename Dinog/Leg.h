#pragma once

#include <Vec3f.h>

struct LegConfig;

class Leg
{
public:
    Leg( const LegConfig& config );
    ~Leg();

    void setPos( const Vec3f& value );
    const Vec3f& getPos() const;
    static Vec3f& getCenter();

    void center();

    // all setPos calls will be applied once commitTransaction is called
    static void startTransaction();
    static void commitTransaction( int msTransactionTime );

private:
    struct Impl;
    Impl* m_impl;
};
