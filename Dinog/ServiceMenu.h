#pragma once

class Controller;

class ServiceMenu
{
public:
    class Observer
    {
    public:
        virtual void setLeg( int i ) = 0;
        virtual void setJoint( int i ) = 0;
        virtual void setTrim( int val ) = 0;
        virtual void save() = 0;
        virtual void evaluate( int x, int y ) = 0;        
        virtual void exit() = 0;
    };

    ServiceMenu( const Controller& controller, Observer* observer );
    ~ServiceMenu();

    void init();
    void update( float dt );

private:
    class State;
    class SetLegState;
    class SetJointState;
    class SetTrimState;
    class EvaluateState;

    const Controller& m_controller;
    Observer* m_observer;
    State** m_states;
    State* m_currentState;
};

