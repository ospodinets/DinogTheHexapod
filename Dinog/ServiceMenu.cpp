#include "ServiceMenu.h"
#include "Common.h"
#include "Controller.h"

class ServiceMenu::State
{
public:
    enum Type
    {
        SetLeg,
        SetJoint,
        Evaluate,

        Total
    };

    State( Type type, ServiceMenu* host )
        : m_type { type }
        , m_host { host }
    {
    }

    virtual void setup() = 0;
    virtual State* update( float dt ) = 0;

protected:    
    Type m_type;
    ServiceMenu* m_host;
};

class ServiceMenu::SetLegState : public  State
{
public:
    SetLegState( ServiceMenu* host );

private: 
    void report();
    void setup() override;
    State* update( float dt ) override;

    int m_leg;
};

class ServiceMenu::SetJointState : public  State
{
public:
    SetJointState( ServiceMenu* host );

private:
    void report();
    void setup() override;
    State* update( float dt ) override;

    int m_joint;
};

class ServiceMenu::EvaluateState : public  State
{
public:
    EvaluateState( ServiceMenu* host );

private:
    void setup() override;
    State* update( float dt ) override;  

    float m_x {}, m_y {};
};

ServiceMenu::SetLegState::SetLegState( ServiceMenu* host )
    : State( SetLeg, host )
    , m_leg { 0 }
{
}

void ServiceMenu::SetLegState::report()
{
    Serial.print( "Current Leg is: " );
    Serial.println( m_leg );
    m_host->m_observer->setLeg( m_leg );
}

void ServiceMenu::SetLegState::setup()
{
    Serial.println( "Set Leg by pressing Prev -> Next (Left stick)" );
    report();
}

ServiceMenu::State* ServiceMenu::SetLegState::update( float dt )
{   
    int setLeg = m_leg;
    auto control = m_host->m_controller.getState();
    switch( control.event )
    {
        case Controller::State::Event::Next:
            ++setLeg;
            break;

        case Controller::State::Event::Prev:                
            --setLeg;
            break;

        case Controller::State::Event::Set:
            {
                return m_host->m_states[SetJoint];
            }
            break;        

        case Controller::State::Event::Exit:
            m_host->m_observer->exitMenu();
            return nullptr;
            break;

        default:
            break;
    }

    if( setLeg != m_leg )
    {
        if( setLeg < 0 )
        {
            setLeg = NUM_LEGS - 1;
        }
        else if ( setLeg >= NUM_LEGS )
        {
            setLeg = 0;
        }
        m_leg = setLeg;
        report();
    }  

    return this;
}

ServiceMenu::SetJointState::SetJointState( ServiceMenu* host )
    : State( SetJoint, host )
    , m_joint { -1 }
{
}

void ServiceMenu::SetJointState::report()
{
    Serial.print( "Current Joint is: " );
    if( m_joint >= 0 )
    {
        Serial.println( m_joint );
    }
    else
    {
        Serial.println( "ALL" );
    }
    m_host->m_observer->setJoint( m_joint );
}

void ServiceMenu::SetJointState::setup()
{
    Serial.println( "Set Leg's joint by pressing Prev -> Next (Left stick)" );
    report();
}

ServiceMenu::SetJointState::State* ServiceMenu::SetJointState::update( float dt )
{
    int setJoint = m_joint;
    auto control = m_host->m_controller.getState();
    switch( control.event )
    {
        case Controller::State::Event::Next:
            ++setJoint;
            break;

        case Controller::State::Event::Prev:
            --setJoint;
            break;

        case Controller::State::Event::Set:
            {
                return m_host->m_states[Evaluate];
            }
            break;

        case Controller::State::Event::Exit:
            {
                return m_host->m_states[SetLeg];
            }
            break;

        default:
            break;
    }

    if( setJoint != m_joint )
    {
        if( setJoint < -1 )
        {
            setJoint = 2;
        }
        else if( setJoint > 2 )
        {
            setJoint = -1;
        }
        m_joint = setJoint;
        report();
    }

    return this;
}

ServiceMenu::EvaluateState::EvaluateState( ServiceMenu* host )
    : State( Evaluate, host )
{
}

void ServiceMenu::EvaluateState::setup()
{
    Serial.println( "Tune trim of given joint or entire leg by moving Right stick" );
    m_x = m_y = 0.0f;
    m_host->m_observer->enterEvaluate();
}

ServiceMenu::State* ServiceMenu::EvaluateState::EvaluateState::update( float dt )
{
    auto& control = m_host->m_controller.getState();

    switch( control.event )
    {
        case Controller::State::Event::Set:            
        case Controller::State::Event::Exit:
            {
                Serial.println( "Exiting menu" );
                m_host->m_observer->exitEvaluate( control.event == Controller::State::Event::Set );
                return m_host->m_states[SetJoint];
            }
            break;

        default:
            {
                static const float rate = 0.5;
                m_x += control.args[0] * rate * dt;
                m_y += control.args[1] * rate * dt;

                if( m_x > 0.5 ) m_x = 0.5;
                if( m_x < -0.5 ) m_x = -0.5;
                if( m_y > 0.5 ) m_y = 0.5;
                if( m_y < -0.5 ) m_y = -0.5;

                m_host->m_observer->evaluate( m_x, m_y );
            }            
            break;
    }
    return this;
}



ServiceMenu::ServiceMenu( const Controller & controller, Observer* observer )
    : m_controller { controller }
    , m_observer { observer }
{
    m_states = new State*[State::Total];

    m_states[State::SetLeg] = new SetLegState( this );
    m_states[State::SetJoint] = new SetJointState( this );
    m_states[State::Evaluate] = new EvaluateState( this );

    m_currentState = nullptr;
}

ServiceMenu::~ServiceMenu()
{
}

void ServiceMenu::init()
{
    auto state = m_states[State::SetLeg];
    state->setup();
    m_currentState = state;
}

void ServiceMenu::update( float dt )
{
    if( m_currentState )
    {
        auto next = m_currentState->update( dt );
        if( next != m_currentState )
        {
            if( next )
                next->setup();
            m_currentState = next;
        }
    }
}
