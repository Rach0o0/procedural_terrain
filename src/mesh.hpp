#pragma once

#include <vector>
#include "heightmap.hpp"

//point 
struct Vec3 {
    float x, y, z;
};

struct Mesh {
    std::vector<Vec3> positions; //list de sommets
    std::vector<int> indices; //3 integers per triangle
};

//transform a Heightmap into a Mesh
//cellSize : largeur d'une case 
//amplitude : hauteur du relief
Mesh makeMesh(const Heightmap& map, float cellSize, float amplitude);
