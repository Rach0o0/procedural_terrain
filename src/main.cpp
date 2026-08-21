#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

#include "image_io.hpp"
#include "plate.hpp"
#include "terrain.hpp"
#include "mesh.hpp"
#include "obj_io.hpp"
#include "erosion.hpp"

const size_t SIZE = 512;
const unsigned int SEED = 1337;

//largeur d'une case du mesh
const float CELL_WORLD = 1.0f;

//amplitude 
const float AMPLITUDE = 150.0f;

void checkMesh(const Mesh& mesh) {
    std::cout << "sommets   " << mesh.positions.size() << "\n";
    std::cout << "triangles " << mesh.indices.size() / 3 << "\n";

    float worst = 0.0f;   // ecart max a une longueur de 1
    int   bad = 0;        // normales pointant vers le bas
    float sum = 0.0f;     // pour la moyenne de n.y

    for (const Vec3& n : mesh.normals) {
        float len = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
        float err = std::fabs(len - 1.0f);
        if (err > worst) worst = err;
        if (n.y <= 0.0f) bad++;
        sum += n.y;
    }

    std::cout << "erreur max longueur : " << worst << "\n";
    std::cout << "normales vers le bas : " << bad << "\n";
    std::cout << "n.y moyen : " << sum / mesh.normals.size() << "\n";
}

int main() {
    TerrainParams params;
    
    Heightmap before = makeTerrain(SIZE,SEED,params);

    Heightmap after = before;
    ErosionParams ep;
    auto t0 = std::chrono::steady_clock::now();
    erode(after, ep, 12345);
    auto t1 = std::chrono::steady_clock::now();
    std::cout << "erosion : " << std::chrono::duration<float>(t1 - t0).count() << " s\n";


    float diff = 0.0f;
    for (size_t y = 0; y < SIZE; y++)
        for (size_t x = 0; x < SIZE; x++)
            diff += std::fabs(after.at(x, y) - before.at(x, y));
    std::cout << "matiere deplacee : " << diff << "\n";

    std::vector<Heightmap> tiles = {before, after};
    Heightmap plate = makePlate(tiles);

    writeGrayscalePng(plate, "output/erosion.png", false);

    std::cout << "ok\n";
    return 0;
}