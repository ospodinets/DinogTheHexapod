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
        virtual void ready() = 0;
        virtual void setControl( const Control& control ) = 0;

        virtual void enterMenu() = 0;
        virtual void enterEvaluation( int leg, int joint ) = 0;
        virtual void evaluate( float x, float y ) = 0;
        virtual void exitEvaluation( bool save ) = 0;
        virtual void exitMenu() = 0;
    };

    InputHandler( Controller& controller, Observer* observer );
    ~InputHandler();

    void update( float dt );
    
private:
    Controller& m_controller;
    Observer* m_observer; 

    bool m_ready { false };
    class ServiceMenuObserver;
    bool m_serviceMenuActive;
    ServiceMenu m_serviceMenu;
};
