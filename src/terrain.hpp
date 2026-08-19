#pragma once
#include "heightmap.hpp"
#include "perlin.hpp"

struct TerrainParams {
    //taille d'une case du bruit, en pixels
    float cellSize = 256.0f;  
    int   octaves = 6;
    float lacunarity = 2.0f;
    float persistence = 0.5f;
    float warp = 0.0f; //0 = no torsion
};

// Carte d'altitudes
Heightmap makeTerrain(size_t size, unsigned int seed, const TerrainParams& params);

// Carte de pente, deduite des derivees du bruit
Heightmap makeSlope(size_t size, unsigned int seed, const TerrainParams& params);