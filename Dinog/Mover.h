#pragma once

struct ControlPose;

class Mover
{
public:
    Mover();
    ~Mover();

    void setControlPose( const ControlPose& state );
    void update( float dt );

private:
    struct Impl;
    Impl* m_impl;
};