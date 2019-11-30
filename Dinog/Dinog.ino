#include "Mover.h"
#include "Controller.h"
#include "InputHandler.h"

namespace
{
    class InputObserver : public InputHandler::Observer
    {
    public:

    private:
        void enableLocomotion( bool enable ) override;
        void setControl( const Control& control ) override;        
        void evaluateLeg( int leg, const Vec3f& pos ) override;
    };
}

unsigned long lastFrame = 0;
Mover mover;
Controller controller;

InputObserver inputObserver;
InputHandler input { controller, &inputObserver };
bool locomotionEnabled = true;

void InputObserver::enableLocomotion( bool enable )
{
    locomotionEnabled = enable;
}

void InputObserver::setControl( const Control& control )
{
    mover.setControl( control );
}

void InputObserver::evaluateLeg( int leg, const Vec3f& pos )
{
    mover.evaluateLeg( leg, pos );
}

// The setup() function runs once each time the micro-controller starts
void setup()
{
    Serial.begin( 9600 );
    Serial.println( "Dinog, The Hexapod Setup" );
    lastFrame = millis();
    controller.init();
    mover.init();
}

// Add the main program code into the continuous loop() function
void loop()
{
    long currFrame = millis();
    if( long delta = currFrame - lastFrame )
    {
        lastFrame = currFrame;
        float dt = delta / 1000.0;

        controller.update( dt );
        input.update( dt );
        mover.update( dt );
    }
}