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
        SetTrim,
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
    void setup() override;
    State* update( float dt ) override;

    int m_leg;
};

class ServiceMenu::SetJointState : public  State
{
public:
    SetJointState( ServiceMenu* host );

private:
    void setup() override;
    State* update( float dt ) override;

    int m_joint;
};

class ServiceMenu::SetTrimState : public  State
{
public:
    SetTrimState( ServiceMenu* host );

private:
    void setup() override;
    State* update( float dt ) override;

    int m_trim;
};

class ServiceMenu::EvaluateState : public  State
{
public:
    EvaluateState( ServiceMenu* host );

private:
    void setup() override;
    State* update( float dt ) override;

    int m_x;
    int m_y;
};

ServiceMenu::SetLegState::SetLegState( ServiceMenu* host )
    : State( SetLeg, host )
    , m_leg { 0 }
{
}

void ServiceMenu::SetLegState::setup()
{
    m_leg = 0;
    m_host->m_observer->setLeg( m_leg );
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
                auto setJoint = m_host->m_states[SetJoint];
                setJoint->setup();
                return setJoint;
            }
            break;        

        case Controller::State::Event::Exit:
            m_host->m_observer->exit();
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
        m_host->m_observer->setLeg( m_leg );
    }  

    return this;
}

ServiceMenu::SetJointState::SetJointState( ServiceMenu* host )
    : State( SetJoint, host )
    , m_joint { 0 }
{
}

void ServiceMenu::SetJointState::setup()
{
    // iterate over -1 for evaluate
    m_joint = -1;
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
                State* next = nullptr;
                if( m_joint == -1 )
                {
                    next = m_host->m_states[Evaluate];                    
                }
                else
                {
                    next = m_host->m_states[SetTrim];
                }
                next->setup();
                return next;
            }
            break;

        default:
            break;
    }

    if( setJoint != m_joint )
    {
        if( setJoint < -1 )
        {
            setJoint = 3;
        }
        else if( setJoint >= 3 )
        {
            setJoint = -1;
        }
        m_joint = setJoint;
        if ( m_joint >= 0)
        m_host->m_observer->setJoint( m_joint );
    }

    return this;
}

ServiceMenu::SetTrimState::SetTrimState( ServiceMenu* host )
    : State( SetTrim, host )
    , m_trim { 0 }
{
}

void ServiceMenu::SetTrimState::setup()
{
    m_trim = 0;
}

ServiceMenu::State* ServiceMenu::SetTrimState::SetTrimState::update( float dt )
{
    auto& control = m_host->m_controller.getState();

    switch( control.event )
    {
        case Controller::State::Event::Value:
            m_trim = control.args[0];
            m_host->m_observer->setTrim( m_trim );
            break;

        case Controller::State::Event::Set:
            m_host->m_observer->save();
        case Controller::State::Event::Exit:
            {
                return m_host->m_states[SetJoint];
            }
            break;
        default:
            break;
    }

    return this;
}

ServiceMenu::EvaluateState::EvaluateState( ServiceMenu* host )
    : State( Evaluate, host )
    , m_x { 0 }
    , m_y { 0 }
{
}

void ServiceMenu::EvaluateState::setup()
{
    m_x = 0;
    m_y = 0;
}

ServiceMenu::State* ServiceMenu::EvaluateState::update( float dt )
{
    auto& control = m_host->m_controller.getState();

    switch( control.event )
    {
        case Controller::State::Event::Value:
            m_x = control.args[0];
            m_y = control.args[1];
            m_host->m_observer->evaluate( m_x, m_y );
            break;

        case Controller::State::Event::Set:
            m_host->m_observer->save();
        case Controller::State::Event::Exit:
            {
                return m_host->m_states[SetLeg];
            }
            break;
        default:
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
    m_states[State::SetTrim] = new SetTrimState( this );
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
        m_currentState = next;
    }
}
