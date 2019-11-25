#pragma once

struct ControlState;

class Mover
{
public:
    Mover();
    ~Mover();

    void setControlState( const ControlState& state );
    void update( float dt );

private:
    struct Impl;
    Impl* m_impl;
};