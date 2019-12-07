#pragma once

#include "SBUS.h"

class Controller
{
public:
    struct State
    {
        enum class Event
        {
            Undefined = 0,
            Control,
            Gait,            
            Menu,

            // These events are sent only in menu mode
            Next,
            Prev,
            Set,
            Exit,
        } event { Event::Undefined };
        
        float args[16] {};
    };

    Controller();
    ~Controller();    

    const State& getState() const
    {
        return m_state;
    }

    void init();
    void update( float dt );
    void exitMenu();

private:  
    struct BoolChannelState
    {
        bool on { false };
        float delay {};
    };

    SBUS m_receiver;
    State m_state;
    uint16_t m_channels[16];

    bool m_initialized { false };
    bool m_menuMode { false };
    
    BoolChannelState m_boolChannels[4];

    
};
