#pragma once

#include <map>
#include <string>

#include "driver/adc.h"

#define V_REF 3.3

typedef struct ThermistorConfig
{
    std::string *name;
    float rDivider;
    double A;
    double B;
    double C;
    float R25;
    float Beta;
    float vRef;
} *pThermistorConfig;

static std::map<const std::string, const ThermistorConfig> knownThermistorConfigs = {
    {"10K", ThermistorConfig{
                // T=100, R=   993
                // T= 34, R= 7,040
                // T=  3, R=26,350
                .name = new std::string("10K"),
                .rDivider = 10000.0,
                .A = 5.507359522044491E-4,
                .B = 3.134967793642773E-4,
                .C = -1.0408993927588221E-7,
                .R25 = 9834.34,
                .Beta = 3401.29,
                .vRef = V_REF,
            }},
    {"100K", ThermistorConfig{
                 // T=100, R=  6,640
                 // T= 34, R= 66,200
                 // T=  3, R=300,000
                 .name = new std::string("100K"),
                 .rDivider = 103000.0,
                 .A = 3.41795556518205E-4,
                 .B = 2.7101265907443527E-4,
                 .C = -6.902798113422121E-8,
                 .R25 = 98016.79,
                 .Beta = 3993.34,
                 .vRef = V_REF,
             }}};
