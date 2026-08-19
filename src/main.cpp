#include <iostream>
#include <string>

#include "heightmap.hpp"
#include "image_io.hpp"
#include "perlin.hpp"

//size of the image
const size_t SIZE = 512;

//taille d'une case du noise en pixels
const float CELL_SIZE = 64.0f;

/*
namespace {

constexpr std::size_t kResolution = 512;

Heightmap makeDiagonalGradient(std::size_t width, std::size_t height) {
    Heightmap map(width, height);

    const float maxCoordinateSum = static_cast<float>((width -1) + (height-1));

    for (size_t y = 0; y<height; ++y){
        for (size_t x = 0; x < width; ++x){
            map.at(x,y) = static_cast<float>(x+y) / maxCoordinateSum;
        }
    }
    return map;
}

}  */

//Perlin noise
Heightmap makePerlin(size_t size, unsigned int seed){
    Perlin p(seed);
    Heightmap map(size, size);

    for (size_t y = 0; y < size; y++){
        for (size_t x = 0; x < size; x++){
            //convertit les pixels en coordonnées de grille du bruit
            float nx = x / CELL_SIZE;
            float ny = y / CELL_SIZE;
            map.at(x,y) = p.noise(nx,ny);
        }
    }

    /*
    DEBUG 
    std::cout << p.noise(3.0f, 5.0f) << "\n";   // doit afficher 0
    std::cout << p.noise(0.0f, 0.0f) << "\n";   // doit afficher 0
    std::cout << p.noise(3.5f, 5.5f) << "\n";   // doit afficher autre chose
    */
    return map;
}

//fabriquer une image large contenant plusieurs versions cote a cote
Heightmap makePlate(unsigned int seed){
    int counts[4] = {1,3,5,8}; //nbr d'octaves à comparer

    Perlin p(seed);
    Heightmap plate(SIZE * 4, SIZE); //4x plus large que haute

    for (int k = 0; k < 4; k++){
        //on stock d'abord la tuile a part pour connaitre son min et max
        float tile[SIZE*SIZE];
        float lo = 1e9f;
        float hi = -1e9f;

        for (size_t y = 0; y < SIZE; y++){
            for (size_t x = 0; x < SIZE; x++){
                float v = p.fbm(x / CELL_SIZE, y / CELL_SIZE, counts[k], 2.0f, 0.5f);
                tile[y * SIZE + x] = v;
                if (v < lo) lo = v;
                if (v > hi) hi = v;
            }
        }

        //on remet la tuile dans [0,1] avec son propre min et max.
        for (size_t y = 0; y < SIZE; y++){
            for (size_t x = 0; x < SIZE; x++){
                float v = (tile[y * SIZE + x] - lo) / (hi - lo);
                plate.at(k * SIZE + x, y) = v;
            }
        }
    }
    return plate;
}

//prend une tuile toute seule, cherche sa valeur la plus basse et sa valuer la plus haute, etale ces valeurs entre 0 et 1
//elle la copie dans la grande image, décalée horizontalement
//chaque tuile utilise toute la gamme du noir et blanc
//C'est pour comparer des tuiles dans une même image
void pasteTile(Heightmap& plate, const Heightmap& tile, size_t offsetX){
    float lo = 1e9f;
    float hi = -1e9f;

    for (size_t y = 0; y < tile.height(); y++){
        for (size_t x = 0; x < tile.width(); x++){
            float v = tile.at(x,y);
            if (v < lo) lo = v;
            if (v > hi) hi = v;
        }
    }

    for (size_t y = 0; y < tile.height(); y++){
        for (size_t x = 0; x < tile.width(); x++){
            plate.at(offsetX + x, y) = (tile.at(x,y) - lo) / (hi - lo);
        }
    }
}

Heightmap makeWarpPlate(unsigned int seed){
    Perlin p(seed);

    int octaves = 6;
    float lac = 2.0f;
    float per = 0.5f;
    float strength = 0.2f;

    Heightmap plain(SIZE,SIZE);
    Heightmap warped(SIZE, SIZE);

    for (size_t y = 0; y < SIZE; y++){
        for (size_t x = 0; x < SIZE; x++){
            float nx = x / CELL_SIZE;
            float ny = y / CELL_SIZE;

            plain.at(x,y) = p.fbm(nx,ny,octaves, lac, per);
            warped.at(x,y) = p.warp(nx, ny, octaves, lac, per, strength);
        }
    }

    Heightmap plate(SIZE * 2, SIZE);
    pasteTile(plate, plain, 0);
    pasteTile(plate, warped, SIZE);
    return plate;

}


int main() {
    const Heightmap map = makeWarpPlate(1337);

   std::string path = "output/warp.png";

   if (!writeGrayscalePng(map, path, true)) {
        std::cerr << "error path";
        return 1;
    }

    std::cout << "written in " << path << "\n";
    return 0;
}