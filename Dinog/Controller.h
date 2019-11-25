#pragma once

struct ControlState;

class Controller
{
public:
    Controller();
    ~Controller();

    const ControlState& getState() const;

    void update( float dt );

private:
    struct Impl;
    Impl* m_impl;
};
