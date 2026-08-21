#pragma once

#include "heightmap.hpp"

struct ErosionParams {
    int droplets = 1000000; //nbr de gouttes lachees
    int maxSteps = 200; //duree de vie max d'une goutte
    float capacity = 30.0f; //facteur d'echelle de la capacite
    float minSlope = 0.001f; //denivele plancher, evite les tas sur le plat
    float erodeRate = 0.3f; //vitesse a laquelle elle creuse
    float depositRate = 0.3f; //vitesse a laquelle elle depose
    float evaporation = 0.02f; //perte d'eau par pas
    float gravity = 4.0f;
};

//erode la map
void erode(Heightmap& map, const ErosionParams& params, unsigned int seed);

