#include <iostream>
#include <vector>

#include "image_io.hpp"
#include "plate.hpp"
#include "terrain.hpp"
#include "mesh.hpp"

const size_t SIZE = 512;
const unsigned int SEED = 1337;

int main() {
    TerrainParams params;

    Heightmap height = makeTerrain(SIZE, SEED, params);
    Heightmap slope  = makeSlope(SIZE, SEED, params);

    std::vector<Heightmap> tiles = {height, slope};
    Heightmap plate = makePlate(tiles);

    // false : makePlate a deja tout mis dans [0,1].
    if (!writeGrayscalePng(plate, "output/slope.png", false)) {
        std::cerr << "erreur d'ecriture\n";
        return 1;
    }

    std::cout << "ok\n";
    return 0;
}