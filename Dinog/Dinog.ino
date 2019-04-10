#include "Mover.h"

unsigned long lastFrame = 0;
Mover* mover = nullptr;

// The setup() function runs once each time the micro-controller starts
void setup()
{
    lastFrame = millis();
    mover = new Mover();
}

// Add the main program code into the continuous loop() function
void loop()
{
    long currFrame = millis();
    if( long delta = currFrame - lastFrame )
    {
        lastFrame = currFrame;
        float dt = delta / 1000.0;

        mover->update( dt );
    }
}