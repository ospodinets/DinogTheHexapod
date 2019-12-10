#include "MathUtils.h"
#include <Arduino.h>
#include <math.h>


float lerp( float v0, float v1, float t )
{
    return ( 1 - t ) * v0 + t * v1;
}

float lerp( float v0, float v1, float t0, float t1, float t )
{
    return ( v0 * ( t1 - t ) + v1 * ( t - t0 ) ) / ( t1 - t0 );
}

float map_f( long x, long in_min, long in_max, float out_min, float out_max, int n_tol )
{
    if( abs( x - in_min ) < n_tol )
        x = in_min;
    if( abs( x - in_max ) < n_tol )
        x = in_max;
    float val = float( x - in_min ) * ( out_max - out_min ) / float( in_max - in_min ) + out_min;
    return val;
}