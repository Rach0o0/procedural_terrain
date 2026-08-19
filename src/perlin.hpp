#pragma once

class Perlin {
public:
    Perlin(unsigned int seed);

    //return altitude du bruit au point (x,y) btwn -0.7 and 0.7
    float noise(float x, float y) const;

    //empile plusieurs octaves de bruit -> result btwn -1 and 1
    float fbm(float x, float y, int octaves, float lacunarity, float persistence) const;

    //fBm avec l'espace tordu, torsion controlée par strength
    float warp(float x, float y, int octaves, float lacunarity, float persistence, float strength) const;
private:
    //table de nombres mélangés, generateur de hasard reproductible
    int perm[512];
};