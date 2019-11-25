#include "Controller.h"
#include "Common.h"
#include "Arduino.h"

namespace
{
}

struct Controller::Impl
{
    ControlState state;    
};

Controller::Controller()
    : m_impl( new Impl )
{
    
}

Controller::~Controller()
{
    delete m_impl;
}

const ControlState& Controller::getState() const
{
    return m_impl->state;
}

void Controller::update( float dt )
{
    auto val = 0; 
    m_impl->state.direction.set( 0, val, 0 );
    
}


