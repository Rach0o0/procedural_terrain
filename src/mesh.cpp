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

//passe doucement de 0 à 1 quand x va de edge0 à edge1
static float smoothstep(float edge0, float edge1, float x) {
    float t = (x - edge0) / (edge1 - edge0);
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    return t * t * (3.0f - 2.0f * t);
}

/* __________________
    COLORS
___________________ */

//mix colors (interpolation)
static Vec3 mixColor(Vec3 a, Vec3 b, float t) {
    Vec3 out;
    out.x = a.x + t * (b.x - a.x);
    out.y = a.y + t * (b.y - a.y);
    out.z = a.z + t * (b.z - a.z);
    return out;
}

static const Vec3 SAND  = {0.76f, 0.70f, 0.50f};
static const Vec3 GRASS = {0.31f, 0.42f, 0.22f};
static const Vec3 ROCK  = {0.42f, 0.40f, 0.38f};
static const Vec3 SNOW  = {0.95f, 0.95f, 0.97f};

static Vec3 pickColor(float height, float ny) {
    //on empile les couches de bas en haut
    Vec3 c = SAND;
    c = mixColor(c, GRASS, smoothstep(0.10f, 0.25f, height));
    c = mixColor(c, ROCK,  smoothstep(0.35f, 0.55f, height));
    c = mixColor(c, SNOW,  smoothstep(0.70f, 0.85f, height));

    //la pente ecrase tout : sur une paroi raide, c'est de la roche
    float steep = smoothstep(0.85f, 0.65f, ny);
    c = mixColor(c, ROCK, steep);

    return c;
}

Mesh makeMesh(const Heightmap& map, float cellSize, float amplitude){
    size_t w = map.width();
    size_t h = map.height();

    Mesh mesh;

    //on reserve la place
    mesh.positions.reserve(w*h);
    mesh.normals.reserve(w*h);
    mesh.colors.reserve(w*h);
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

            mesh.colors.push_back(pickColor(map.at(x,y), n.y));
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