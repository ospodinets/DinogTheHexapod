#include "Mover.h"
#include "Controller.h"

unsigned long lastFrame = 0;
Mover mover;
Controller controller;

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
        mover.setControlState( controller.getState() );
        mover.update( dt );
    }
}