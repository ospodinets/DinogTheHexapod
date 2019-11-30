#include "Controller.h"

Controller::Controller()
    : m_receiver( Serial2 )
{    
}

Controller::~Controller()
{
    
}

void Controller::init()
{
    m_receiver.begin();
}

void Controller::update( float dt )
{

    
}


