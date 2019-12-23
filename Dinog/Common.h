#pragma once

static const int NUM_LEGS = 6;

struct Control
{   
    float elevation { 0.0f };
    float torque { 0.0f };
    float forward { 0.0f };
    float right { 0.0f };
};
