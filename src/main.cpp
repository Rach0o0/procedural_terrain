#include <iostream>
#include <vector>
#include <cmath>

#include "image_io.hpp"
#include "plate.hpp"
#include "terrain.hpp"
#include "mesh.hpp"
#include "obj_io.hpp"

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
    Heightmap map = makeTerrain(SIZE, SEED, params);
    Mesh mesh = makeMesh(map, CELL_WORLD, AMPLITUDE);

    checkMesh(mesh);

    if (!writeOBJ(mesh, "output/terrain.obj")){
        std::cerr << "erreur d'écriture de l'obj\n";
        return 1;
    }

    //heighmap altitude vs slope
    Heightmap slope  = makeSlope(SIZE, SEED, params);
    std::vector<Heightmap> tiles = {map, slope};
    Heightmap plate = makePlate(tiles);

    // false : makePlate a deja tout mis dans [0,1].
    if (!writeGrayscalePng(plate, "output/slope.png", false)) {
        std::cerr << "erreur d'ecriture\n";
        return 1;
    }

    std::cout << "ok\n";
    return 0;
}