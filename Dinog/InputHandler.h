#pragma once

#include "ServiceMenu.h"
#include <Vec3f.h>

class Control;
class Controller;

class InputHandler
{
public:
    class Observer
    {
    public:
        virtual void enableLocomotion( bool enable ) = 0;
        virtual void setControl( const Control& control ) = 0;
        //virtual void setGait( int ) = 0;        
        virtual void evaluateLeg( int leg, const Vec3f& pos ) = 0;
    };

    InputHandler( const Controller& controller, Observer* observer );
    ~InputHandler();

    void update( float dt );
    
private:
    const Controller& m_controller;
    Observer* m_observer; 

    class ServiceMenuObserver;
    bool m_serviceMenuActive;
    ServiceMenu m_serviceMenu;
};
