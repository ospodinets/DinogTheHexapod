#pragma once

#include "SBUS.h"

class Controller
{
public:
    struct State
    {
    };

    Controller();
    ~Controller();    

    const State& getState() const
    {
        return m_state;
    }

    void init();
    void update( float dt );

private:
    SBUS m_receiver;
    State m_state;
};
