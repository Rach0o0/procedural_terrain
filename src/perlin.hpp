#pragma once

class Perlin {
public:
    Perlin(unsigned int seed);

    //return altitude du bruit au point (x,y) btwn -0.7 and 0.7
    float noise(float x, float y) const;

private:
    //table de nombres mélangés, generateur de hasard reproductible
    int perm[512];
};