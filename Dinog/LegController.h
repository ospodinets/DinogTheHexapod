#pragma once

class Vec3f;
struct LegConfig;

class LegController
{
public:
    LegController( const LegConfig& config );
    ~LegController();

    void setLocomotionVector( const Vec3f& val );
    void setPhaze( float phaze );

private:
    struct Impl;
    Impl* m_impl;
};
