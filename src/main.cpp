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

int main() {
    const Heightmap map = makePerlin(SIZE, 1337);

   std::string path = "output/perlin.png";

   if (!writeGrayscalePng(map, path, true)) {
        std::cerr << "error path";
        return 1;
    }

    std::cout << "written in " << path << "\n";
    return 0;
}