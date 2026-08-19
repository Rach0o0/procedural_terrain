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

//dérivée de fade
static float fadeD(float t){
    return 30 * t * t * (t - 1) * (t - 1);
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

float Perlin::fbm(float x, float y, int octaves, float lacunarity, float persistence) const{
    //sum of octaves
    float total = 0.0f;
    //a quel point cette octave est serrée
    float freq = 1.0f;
    //a quel point cette octave compte
    float amp = 1.0f;
    //sum of amplitudes
    float ampSum = 0.0f;

    for (int i = 0; i < octaves; i++){
        //bruit plus serré, on multiplie les coordonnées
        total += noise(x*freq, y*freq)*amp;
        //on retient l'amplitude
        ampSum += amp;
        //octave suivante
        freq *= lacunarity;
        amp *= persistence;
    }

    //faut diviser sinon le resultat grandirait avec le nbr d'octaves
    return total / ampSum;
}

float Perlin::warp(float x, float y, int octaves, float lacunarity, float persistence, float strength) const {
    //on build un vecteur de décalage (qx, qy) avec deux fbm
    //two random constants 5.2 & 1.3 to see noise at two different places

    float qx = fbm(x,y,octaves,lacunarity,persistence);
    float qy = fbm(x+5.2f,y+1.3f,octaves,lacunarity,persistence);

    //evalue le bruit final au point déplacé
    return fbm(x + strength * qx, y + strength * qy, octaves, lacunarity, persistence);

    //ça coute 3x plus cher qu'un fBm simple
}

NoiseD Perlin::noiseD(float x, float y) const {
    float fx = std::floor(x);
    float fy = std::floor(y);

    int cellX = (int)fx & 255;
    int cellY = (int)fy & 255;

    float px = x - fx;
    float py = y - fy;

    float u = fade(px);
    float v = fade(py);
    float du = fadeD(px); //derivative
    float dv = fadeD(py); //derivative

    int left  = perm[cellX] + cellY;
    int right = perm[cellX + 1] + cellY;

    int h00 = perm[left];   
    int h10 = perm[right];      
    int h01 = perm[left + 1];    
    int h11 = perm[right + 1]; 

    float a = cornerValue(h00, px,     py);
    float b = cornerValue(h10, px - 1, py);
    float c = cornerValue(h01, px,     py - 1);
    float d = cornerValue(h11, px - 1, py - 1);

    //on recupere les fleches 
    float ax = dirX[h00 & 7], ay = dirY[h00 & 7];
    float bx = dirX[h10 & 7], by = dirY[h10 & 7];
    float cx = dirX[h01 & 7], cy = dirY[h01 & 7];
    float dx = dirX[h11 & 7], dy = dirY[h11 & 7];

    //rewrite the mix in a developped way, la derivée s'ecrit facilement sous cette forme
    float k1 = b - a;
    float k2 = c - a;
    float k3 = a - b - c + d;

    NoiseD out;
    out.value = a + k1 * u + k2 * v + k3 * u * v;

    //derivative of the expression
    out.dx = ax + (bx - ax) * u + (cx - ax) * v + (ax - bx - cx + dx) * u * v
             + du * (k1 + k3 * v);

    out.dy = ay + (by - ay) * u + (cy - ay) * v + (ay - by - cy + dy) * u * v
             + dv * (k2 + k3 * u);

    return out;
}

NoiseD Perlin::fbmD(float x, float y, int octaves, float lacunarity, float persistence) const {
    float total = 0.0f;
    float totalDx = 0.0f;
    float totalDy = 0.0f;

    float freq = 1.0f;
    float amp = 1.0f;
    float ampSum = 0.0f;

    for (int i = 0; i < octaves; i++){
        NoiseD n = noiseD(x * freq, y * freq);

        total += n.value * amp;

        totalDx += n.dx * amp * freq;
        totalDy += n.dy * amp * freq;

        ampSum += amp;
        freq *= lacunarity;
        amp *= persistence;
    }

    NoiseD out;
    out.value = total / ampSum;
    out.dx = totalDx / ampSum;
    out.dy = totalDy / ampSum;
    return out;
}