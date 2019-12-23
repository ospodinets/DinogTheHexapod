#include "Mover.h"
#include "Controller.h"
#include "InputHandler.h"

#include <MathUtils.h>

namespace
{
    class InputObserver : public InputHandler::Observer
    {
    public:

    private:
        void ready() override;
        void setControl( const Control& control ) override;
        void enterMenu() override;
        void enterEvaluation( int leg, int joint ) override;
        void evaluate( float x, float y ) override;
        void exitEvaluation( bool save ) override;
        void exitMenu() override;
    };

    int trimValue( float x )
    {
        return int( x * 30 );
    }

    Vec3f evalValue( float x, float y )
    {
        return Vec3f( x * 100.0f, y * 100.0f, 0.0f );
    }
}

unsigned long lastFrame = 0;
Mover mover;
Controller controller;

InputObserver inputObserver;
InputHandler input { controller, &inputObserver };

int legTrimming {};
int jointTrimming {};
Leg::Config untouched;
bool changed {};
float prev_x {}, prev_y {};

void InputObserver::ready()
{
    mover.enableLocomotion( true );
}

void InputObserver::setControl( const Control& control )
{
    mover.setControl( control );
}

void InputObserver::enterMenu()
{
    mover.enableLocomotion( false );    
}

void InputObserver::enterEvaluation( int leg, int joint )
{
    legTrimming = leg;
    jointTrimming = joint;
    untouched = Leg::getConfig( leg );

    if( jointTrimming >=0 )
        Serial.print( "Current trim is: " );

    switch( jointTrimming )
    {
        case 0:
            Serial.println( untouched.coxaTrim );
            break;

        case 1:
            Serial.println( untouched.femurTrim );
            break;

        case 2:
            Serial.println( untouched.tibiaTrim );
            break;

        default:
            break;
    }
}

void InputObserver::evaluate( float x, float y )
{
    if( fabs( prev_x - x ) < F_TOLERANCE || fabs( prev_y - y ) < F_TOLERANCE )
        return;

    prev_x = x;
    prev_y = y;

    auto& config = Leg::getConfig( legTrimming );

    if( jointTrimming < 0 )
    {
        mover.evaluateLeg( legTrimming, evalValue( x, y ) );
    }
    else
    {
        switch( jointTrimming )
        {
            case 0:
                config.coxaTrim = untouched.coxaTrim + trimValue( x );
                break;

            case 1:
                config.femurTrim = untouched.femurTrim + trimValue( x );
                break;

            case 2:
                config.tibiaTrim = untouched.tibiaTrim + trimValue( x );
                break;

            default:
                break;
        }
        mover.centerLeg( legTrimming );
    }    
}
void InputObserver::exitEvaluation( bool save )
{
    if( save )
    {
        untouched = Leg::getConfig( legTrimming );
        changed = true;
    }
    mover.centerLeg( legTrimming );
}
void InputObserver::exitMenu()
{
    if( changed )
    {
        Leg::saveConfig();
    }
    mover.enableLocomotion( true );
}

// The setup() function runs once each time the micro-controller starts
void setup()
{
#ifdef DEBUG_TRACE
    Serial.begin( 9600 );
    Serial.println( "Dinog, The Hexapod Setup" );
#endif

    Leg::loadConfig();
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