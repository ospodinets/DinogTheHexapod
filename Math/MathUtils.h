#pragma once

#include <math.h>

static const float F_TOLERANCE = 1e-03;

float lerp( float v0, float v1, float t );

float lerp( float v0, float v1, float t0, float t1, float t );

float map_f( long x, long in_min, long in_max, float out_min, float out_max, int n_tol );