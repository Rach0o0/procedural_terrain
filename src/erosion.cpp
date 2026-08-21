#include "erosion.hpp"

#include <cmath>
#include <random>
#include <iostream>

//on modifie la map sur place
void erode(Heightmap& map, const ErosionParams& params, unsigned int seed) {
    int w = (int)map.width();
    int h = (int)map.height();

    std::mt19937 rng(seed);
    //on tire des entiers entre 1 et w-2 : et on exclus les bords
    std::uniform_int_distribution<int> pickX(1, w - 2);
    std::uniform_int_distribution<int> pickY(1, h - 2);

    long long totalSteps = 0;
    //iteration = une goutte
    for (int d = 0; d < params.droplets; d++) {

        //nouvelle goutte, posée au hasard
        int x = pickX(rng);
        int y = pickY(rng);

        float water = 1.0f; //volume d'eau
        float speed = 1.0f; // vitesse
        float sediment = 0.0f; //ce qu'elle porte

        

        //la vie de la goutte
        for (int step = 0; step < params.maxSteps; step++) {

            totalSteps++;

            //a quelle altitude on est
            float here = map.at((size_t)x, (size_t)y);

            //on cherche le plus bas des 8 voisins.

            //hypothese : le plus bas c'est moi
            int bestX = x;
            int bestY = y;
            float bestH = here;

            //on voit les 8 cases autour
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    
                    if (dx == 0 && dy == 0) continue;

                    int nx = x + dx;
                    int ny = y + dy;
                    //si le voisin est hors de la carte, on ignore
                    if (nx < 0 || ny < 0 || nx >= w || ny >= h) continue;
                    
                    float nh = map.at((size_t)nx, (size_t)ny);
                    
                    //strictement plus bas: on retient ce voisin
                    if (nh < bestH) {
                        bestH = nh;
                        bestX = nx;
                        bestY = ny;
                    }
                }
            }

            //aucun voisin plus bas : on est dans un creux
            //la goutte s'arrete et lache tout ce qu'elle porte
            if (bestX == x && bestY == y) {
                map.at((size_t)x, (size_t)y) += sediment;
                break;
            }

            //le denivele qu'elle va descendre
            float drop = here - bestH; //altitude - altitude du voisin le plus bas

            //ce que la goutte peut porter a cet instant.
            float slope = drop;
            if (slope < params.minSlope) slope = params.minSlope;
            //formule de la capacity
            float capacity = slope * speed * water * params.capacity;

            //creuser ou déposer
            if (sediment > capacity) {
                //porte plus qu'elle ne peut, lache l'excedent
                float amount = (sediment - capacity) * params.depositRate;
                //le depot se fait sur la case qu'on quitte
                map.at((size_t)x, (size_t)y) += amount;
                sediment -= amount;
            } else {
                //il lui reste de la place, elle arrache et charge
                float amount = (capacity - sediment) * params.erodeRate;

                //securite pour pas avoir de gruyere
                if (amount > drop) amount = drop;

                map.at((size_t)x, (size_t)y) -= amount;
                sediment += amount;
            }

            //elle accelere en tombant. La racine vient de l'energie cinetique.
            speed = std::sqrt(speed * speed + drop * params.gravity);

            //elle perd de l'eau.
            water *= (1.0f - params.evaporation);

            //elle se deplace.
            x = bestX;
            y = bestY;

            
        }
    }
    std::cout << "pas moyens par goutte : "
              << (double)totalSteps / params.droplets << "\n";
}