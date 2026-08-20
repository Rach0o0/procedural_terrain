#include "mesh.hpp"

#include <cmath>

//return height
static float heightAt(const Heightmap& map, int x, int y, float amplitude){
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= (int)map.width()) x = (int)map.width()-1;
    if (y >= (int)map.height()) y = (int)map.height() - 1;

    return map.at((size_t)x, (size_t)y)*amplitude;


}

Mesh makeMesh(const Heightmap& map, float cellSize, float amplitude){
    size_t w = map.width();
    size_t h = map.height();

    Mesh mesh;

    //on reserve la place
    mesh.positions.reserve(w*h);
    mesh.normals.reserve(w*h);
    mesh.indices.reserve((w-1)*(h-1)*6);

    //sommets
    //un sommet par case, dans l'ordre de la map
    // (y * w + x) -> case (x,y)
    for (size_t y = 0; y < h; y++){
        for (size_t x = 0; x < w; x++){
            Vec3 p;
            p.x = x * cellSize;
            p.y = map.at(x,y) * amplitude;
            p.z = y * cellSize;
            mesh.positions.push_back(p);

            float slopeX = (heightAt(map, (int)x + 1, (int)y, amplitude) - heightAt(map, (int)x - 1, (int)y, amplitude)) / (2.0f * cellSize);
            float slopeZ = (heightAt(map, (int)x, (int)y + 1, amplitude) - heightAt(map, (int)x, (int)y - 1, amplitude)) / (2.0f * cellSize);
            
            //normal penche à l'opposée de la montée
            Vec3 n;
            n.x = -slopeX;
            n.y = 1.0f;
            n.z = -slopeZ;

            //length = 1
            float len = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
            n.x /= len;
            n.y /= len;
            n.z /= len;

            mesh.normals.push_back(n);
        }
    }

    //triangles
    //on s'arrete à w-1 et h-1 (grille de w sommets contient w-1 cases)
    for (size_t y = 0; y < h-1; y++){
        for (size_t x = 0; x<w -1; x++){
            //les quatre coins de cette case
            int topLeft = (int)(y * w + x);
            int topRight = topLeft + 1;
            int bottomLeft = (int)((y + 1) * w + x);
            int bottomRight = bottomLeft + 1;

            //premier triangle
            mesh.indices.push_back(topLeft);
            mesh.indices.push_back(bottomLeft);
            mesh.indices.push_back(topRight);

            //deuxieme triangle
            mesh.indices.push_back(topRight);
            mesh.indices.push_back(bottomLeft);
            mesh.indices.push_back(bottomRight);
        }
    }

    return mesh;
}