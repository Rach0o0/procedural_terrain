#include "terrain.hpp"
#include <cmath>


Heightmap makeTerrain(size_t size, unsigned int seed, const TerrainParams& params) {
    //construit le générateur une seule fois avec une seed -> meme table 
    Perlin p(seed);
    //carte vide qu'on va remplir
    Heightmap map(size, size);

    //on parcourt tous les pixels
    for (size_t y = 0; y < size; y++) {
        for (size_t x = 0; x < size; x++) {
            
            //conversion pixel -> coordonnée de bruit
            //cellSize : combien de pixels fait une case
            float nx = x / params.cellSize; //nb de case horizontalement 
            float ny = y / params.cellSize;

            float v;

            //si warp = 0, pas besoin de 3 appels fbm pour rien
            if (params.warp > 0.0f) {
                v = p.warp(nx, ny, params.octaves, params.lacunarity, params.persistence, params.warp);
            } else {
                v = p.fbm(nx, ny, params.octaves, params.lacunarity, params.persistence);
            }
            //on range l'altitude dans la case
            map.at(x, y) = v;
        }
    }
    return map;
}

Heightmap makeSlope(size_t size, unsigned int seed, const TerrainParams& params) {
    Perlin p(seed);
    Heightmap map(size, size);

    for (size_t y = 0; y < size; y++) {
        for (size_t x = 0; x < size; x++) {
            float nx = x / params.cellSize;
            float ny = y / params.cellSize;

            //fbmD renvoie 3 nombres : altitude, dx, dy
            NoiseD n = p.fbmD(nx, ny, params.octaves, params.lacunarity, params.persistence);

            //on garde que la longuer de la fleche (dx, dy)
            map.at(x, y) = std::sqrt(n.dx * n.dx + n.dy * n.dy);
        }
    }
    return map;
}