#include "plate.hpp"

//copie une tuile dans la planche en etalant ses valeurs entre 0 et 1.
static void pasteTile(Heightmap& plate, const Heightmap& tile, size_t offsetX) {
    float lo = 1e9f;
    float hi = -1e9f;

    //premier passage : on cherche le min / max de cette tuile
    for (size_t y = 0; y < tile.height(); y++) {
        for (size_t x = 0; x < tile.width(); x++) {
            float v = tile.at(x, y);
            if (v < lo) lo = v;
            if (v > hi) hi = v;
        }
    }

    //ecart entre le plus bas et le plus haut
    float range = hi - lo;
    //case range = 0 
    if (range < 1e-9f) range = 1.0f;  

    //deuxieme passage : on etale et on copie 
    for (size_t y = 0; y < tile.height(); y++) {
        for (size_t x = 0; x < tile.width(); x++) {
            //(v-lo) / range -> envoie le plus bas sur 0 et le plus haut sur 1
            //offsetX decale l'ecriture vers la droite dans la grande image
            plate.at(offsetX + x, y) = (tile.at(x, y) - lo) / range;
        }
    }
}

Heightmap makePlate(const std::vector<Heightmap>& tiles) {
    //on suppose que toutes les tuiles font la même taille
    
    size_t w = tiles[0].width();
    size_t h = tiles[0].height();

    Heightmap plate(w * tiles.size(), h);
    for (size_t i = 0; i < tiles.size(); i++) {
        pasteTile(plate, tiles[i], i * w);
    }
    return plate;
}