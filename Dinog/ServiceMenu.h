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
        virtual void enterEvaluate() = 0;        
        virtual void evaluate( float x, float y ) = 0;
        virtual void exitEvaluate( bool save ) = 0;
        virtual void exitMenu() = 0;
    };

    ServiceMenu( const Controller& controller, Observer* observer );
    ~ServiceMenu();

    void init();
    void update( float dt );

private:
    class State;
    class SetLegState;
    class SetJointState;
    class EvaluateState;

    const Controller& m_controller;
    Observer* m_observer;
    State** m_states;
    State* m_currentState;
};

