#include "Controller.h"
#include "Common.h"

#include <MathUtils.h>

namespace
{
    const int s_NextChannelId = 1;
    const int s_PrevChannelId = 1;
    const int s_SetChannelId = 0;
    const int s_ExitChannelId = 0;
    const int s_ValueXChannelId = 2;
    const int s_ValueYChannelId = 3;    

    const int s_minUntrimmed = 180;
    const int s_maxUntrimmed = 1800; 

    int n_tol = 10;

    const float s_delay = 0.05f;
}


Controller::Controller()
    : m_receiver( Serial1 )
    , m_tmp { m_buff1 }
    , m_curr { m_buff2 }
{       
}

Controller::~Controller()
{
    
}

void Controller::init()
{
#ifdef DEBUG_TRACE
    Serial.println( "Initialize controller" );
#endif
    m_receiver.begin();
}

void Controller::update( float dt )
{
    auto prev = m_state;

    m_state.event = State::Event::Undefined;

    bool failsafe, lost;
    if( m_receiver.read( m_tmp, &failsafe, &lost ) )
    {
        if( !failsafe && !lost )
        {
            swap();

            if( !m_initialized )
            {
                m_initialized = true;
                // enter service menu if controller initialized with high value for the first channel
                m_menuMode = m_curr[3] >= s_maxUntrimmed;

                
#ifdef DEBUG_TRACE
                Serial.println( "Controller initialized" );
#endif

                if( m_menuMode )
                {
                    m_state.event = State::Event::Menu;
#ifdef DEBUG_TRACE
                    Serial.println( "\tmenu mode: on" );
#endif                   

                    m_boolChannels[0].on = abs( m_curr[s_NextChannelId] - s_maxUntrimmed ) < n_tol;
                    m_boolChannels[1].on = abs( m_curr[s_PrevChannelId] - s_minUntrimmed ) < n_tol;
                    m_boolChannels[2].on = abs( m_curr[s_SetChannelId] - s_maxUntrimmed ) < n_tol;
                    m_boolChannels[3].on = abs( m_curr[s_ExitChannelId] - s_minUntrimmed ) < n_tol;
                    return;
                }
            }
        }        
    }

    if( !m_initialized )
        return;

    if( m_menuMode )
    {
        bool isOn = abs( m_curr[s_NextChannelId] - s_maxUntrimmed ) < n_tol;
        if( isOn != m_boolChannels[0].on )
        {
            if( !isOn && m_boolChannels[0].delay > s_delay )
            {
                m_state.event = State::Event::Next;
            }
            m_boolChannels[0].on = isOn;
            m_boolChannels[0].delay = 0.0f;
        }
        else
        {
            m_boolChannels[0].delay += dt;
        }

        isOn = abs( m_curr[s_PrevChannelId] - s_minUntrimmed ) < n_tol;
        if( isOn != m_boolChannels[1].on )
        {
            if( !isOn && m_boolChannels[1].delay > s_delay )
            {
                m_state.event = State::Event::Prev;
            }
            m_boolChannels[1].on = isOn;
            m_boolChannels[1].delay = 0.0f;
        }
        else
        {
            m_boolChannels[1].delay += dt;
        }

        isOn = abs( m_curr[s_SetChannelId] - s_maxUntrimmed ) < n_tol;
        if( isOn != m_boolChannels[2].on )
        {
            if( !isOn && m_boolChannels[2].delay > s_delay )
            {
                m_state.event = State::Event::Set;
            }
            m_boolChannels[2].on = isOn;
            m_boolChannels[2].delay = 0.0f;
        }
        else
        {
            m_boolChannels[2].delay += dt;
        }

        isOn = abs( m_curr[s_ExitChannelId] - s_minUntrimmed ) < n_tol;
        if( isOn != m_boolChannels[3].on )
        {
            if( !isOn && m_boolChannels[3].delay > s_delay )
            {
                m_state.event = State::Event::Exit;
            }
            m_boolChannels[3].on = isOn;
            m_boolChannels[3].delay = 0.0f;
        }
        else
        {
            m_boolChannels[3].delay += dt;
        }  

        m_state.args[0] = map_f( m_curr[s_ValueXChannelId], s_minUntrimmed, s_maxUntrimmed, -0.5f, 0.5f, n_tol );
        m_state.args[1] = map_f( m_curr[s_ValueYChannelId], s_minUntrimmed, s_maxUntrimmed, -0.5f, 0.5f, n_tol );
    }
    else
    {
        m_state.event = State::Event::Control;

        m_state.args[0] = map_f( m_curr[0], s_minUntrimmed, s_maxUntrimmed, 0.0f, 1.0f, n_tol );
        m_state.args[1] = map_f( m_curr[1], s_minUntrimmed, s_maxUntrimmed, -0.5f, 0.5f, n_tol );
        m_state.args[2] = map_f( m_curr[2], s_minUntrimmed, s_maxUntrimmed, -0.5f, 0.5f, n_tol );
        m_state.args[3] = map_f( m_curr[3], s_minUntrimmed, s_maxUntrimmed, -0.5f, 0.5f, n_tol );
        m_state.args[4] = map_f( m_curr[4], s_minUntrimmed, s_maxUntrimmed, -0.5f, 0.5f, n_tol );
    }

}

void Controller::exitMenu()
{
    m_menuMode = false;
}

void Controller::swap()
{
    auto tmp = m_tmp;
    m_tmp = m_curr;
    m_curr = tmp;
}


