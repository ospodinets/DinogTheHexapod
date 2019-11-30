#include "InputHandler.h"
#include "ServiceMenu.h"
#include "Controller.h"
#include "Common.h"

namespace
{
    Control parseControl( const Controller::State& state )
    {
        Control ctrl;
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
        m_untouched = getLegConfig( m_currLeg );        
    }

    void setJoint( int i ) override 
    {
        m_currJoint = i;
    }

    void setTrim( int val ) override 
    {
        auto& config = getLegConfig( m_currLeg );

        switch( m_currJoint )
        {
            case 0:
                config.coxaTrim = m_untouched.coxaTrim + val;
                break;

            case 1:
                config.femurTrim = m_untouched.femurTrim + val;
                break;

            case 2:
                config.tibiaTrim = m_untouched.tibiaTrim + val;
                break;

            default:
                break;
        }
    }

    void save() override 
    {
        m_changed = true;
        m_untouched = getLegConfig( m_currLeg );
    }

    void evaluate( int x, int y ) override
    {
        
    }
    
    void exit() override 
    {
        if( m_changed )
        {
            saveConfig();
            m_changed = true;
        }
            
        m_host->m_observer->enableLocomotion( true );
    }

private:
    InputHandler* m_host;
    int m_currLeg;
    int m_currJoint;

    LegConfig m_untouched;
    bool m_changed {};
};


InputHandler::InputHandler( const Controller & controller, Observer * observer )
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

    switch( control.event )
    {
        case Controller::State::Event::Control:
            m_observer->setControl( parseControl( control ) );
            break;

        case Controller::State::Event::Gait:
            break;

        case Controller::State::Event::Menu:
            m_observer->enableLocomotion( false );
            m_serviceMenu.init();
            break;

        default:
            break;
    }

    if( m_serviceMenuActive )
    {
        m_serviceMenu.update( dt );
    }
}


