#include "InputHandler.h"
#include "ServiceMenu.h"
#include "Controller.h"
#include "Common.h"

namespace
{
    Control parseControl( const Controller::State& state )
    {
        Control ctrl;

        ctrl.thr = state.args[0];        
        ctrl.torque = state.args[1];
        if( fabs( ctrl.torque ) < 0.05 )
            ctrl.torque = 0.0f;
        ctrl.dir.set( - state.args[3], - state.args[2], 1.0f );

        ctrl.height = state.args[4];
        return ctrl;
    }
}

class InputHandler::ServiceMenuObserver : public ServiceMenu::Observer
{
public:
    ServiceMenuObserver( InputHandler* host )
        : m_host { host }
    { }

private:

    void setLeg( int i ) override
    {
        m_currLeg = i;
    }

    void setJoint( int i ) override
    {
        m_currJoint = i;
    }

    void enterEvaluate() override
    {
        m_host->m_observer->enterEvaluation( m_currLeg, m_currJoint );
    }

    void evaluate( float x, float y ) override
    {
        m_host->m_observer->evaluate( x, y );
    }

    void exitEvaluate( bool save ) override
    {
        m_host->m_observer->exitEvaluation( save );
    }

    void exitMenu() override
    {
        m_host->m_observer->exitMenu();
        m_host->m_serviceMenuActive = false;
        m_host->m_controller.exitMenu();
    }    

private:
    InputHandler* m_host;
    int m_currLeg;
    int m_currJoint;    
};


InputHandler::InputHandler( Controller & controller, Observer * observer )
    : m_controller { controller }
    , m_observer { observer }
    , m_serviceMenuActive { false }
    , m_serviceMenu { controller, new ServiceMenuObserver{ this } }
{
}

InputHandler::~InputHandler()
{
}

void InputHandler::update( float dt )
{
    auto control = m_controller.getState();
    if( m_serviceMenuActive )
    {
        m_serviceMenu.update( dt );
    }
    else
    {
        switch( control.event )
        {
            case Controller::State::Event::Control:
                if( !m_ready )
                {
                    m_observer->ready();
                    m_ready = true;
                }

                m_observer->setControl( parseControl( control ) );
                break;

            case Controller::State::Event::Gait:
                break;

            case Controller::State::Event::Menu:
                Serial.println( "Service menu active" );
                m_observer->enterMenu();
                m_serviceMenu.init();
                m_serviceMenuActive = true;
                break;

            default:
                break;
        }
    }    
}


