#pragma once

#include <Vec3.h>

class Leg
{
public:
    Leg( int coxaPin, int femurPin, int tibiaPin, bool inverted );
    ~Leg();

    void setPos( const Vec3f& value );
    const Vec3f& getPos() const;

    // all setPos calls will be applied once commitTransaction is called
    static void startTransaction();
    static void commitTransaction( int msTransactionTime );

private:
    struct Impl;
    Impl* m_impl;
};
