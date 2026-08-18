#include "perlin.hpp"

#include <algorithm>
#include <cmath>
#include <random>

/*
les 8 directions possibles pour une fleche de coin : 
4 straight lines (haut, bas, gauche, droite)
4 diagonales : length 0.707
tous les vecteurs de même longueur
*/
static const float dirX[8] = { 1, -1, 0,  0,  0.707f, -0.707f,  0.707f, -0.707f };
static const float dirY[8] = { 0,  0, 1, -1,  0.707f,  0.707f, -0.707f, -0.707f };

/*
courbe de lissage de Perlin
transforme t (btwn 0 and 1) en une verstion "adoucie"
*/
static float fade(float t){
    return t * t * t * (t * (t * 6 -15) + 10);
}

//mix a and b 
static float mix(float a, float b, float t){
    return a + t * (b-a);
}

/*
l'avis d'un coin sur l'altitude
hash choisit la fleche du coin, (dx, dy) est la direction du cion vers le point
on calcule le produit scalaire 
*/
static float cornerValue(int hash, float dx, float dy){
    int i = hash & 7; //nb btwn 0 & 7
    return dirX[i] * dx + dirY[i] * dy;
}

Perlin::Perlin(unsigned int seed){
    //liste 0, 1, ..., 255
    int base[256];
    for (int i = 0; i< 256; i++){
        base[i] = i;
    }

    //we mix, seed décide de l'ordre du mix
    std::mt19937 rng(seed); //générateur de nombres aléatoires selon la seed
    std::shuffle(base, base+256, rng);

    /*
    on recopie la liste 2 fois de suite dans perm
    perm[a] + b peut monter jusqu'a 510
    */
    for (int i = 0; i < 256; i++) {
        perm[i] = base[i]; 
        perm[i+256] = base[i];
    }
}

float Perlin::noise(float x, float y) const {
    //coin haut-gauche de la case
    float fx = std::floor(x);
    float fy = std::floor(y);

    //on le ramene entre 0 et 255
    int cx = (int)fx & 255;
    int cy = (int)fy & 255;

    //on est où à l'intérieur de la case (btwn 0 & 1)
    float px = x - fx;
    float py = y - fy;

    //we fade it
    float u = fade(px);
    float v = fade(py);

    //pour chaque coin, on prend un vecteur au hasard
    int left = perm[cx] + cy;
    int right = perm[cx+1] + cy;

    int hashTopLeft = perm[left];
    int hashTopRight = perm[right];
    int hashBottomLeft = perm[left+1];
    int hashBottomRight = perm[right+1];

    //on demande l'avis a chaqeu coin
    float topLeft     = cornerValue(hashTopLeft,     px,     py);
    float topRight    = cornerValue(hashTopRight,    px - 1, py);
    float bottomLeft  = cornerValue(hashBottomLeft,  px,     py - 1);
    float bottomRight = cornerValue(hashBottomRight, px - 1, py - 1);

    //we mix
    float top    = mix(topLeft,    topRight,    u);
    float bottom = mix(bottomLeft, bottomRight, u);

    return mix(top, bottom, v);
}