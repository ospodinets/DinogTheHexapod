#pragma once

#include "SBUS.h"

class Controller
{
public:
    struct State
    {
        enum class Event
        {
            Undefined,
            Control,
            Gait,            
            Menu,

            // These events are sent only in menu mode
            Next,
            Prev,
            Set,
            Exit,
            Value,
        } event { Event::Undefined };
        
        int args[16] {};
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
