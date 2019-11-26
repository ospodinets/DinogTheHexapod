#pragma once

#include "SBUS.h"

#include "Common.h"

struct ControlState;

class Controller
{
public:
    Controller();
    ~Controller();    

    const ControlState& getState() const
    {
        return m_state;
    }

    void init();
    void update( float dt );

private:
    SBUS m_receiver;
    ControlState m_state;
};
