#include "obj_io.hpp"

#include <fstream>

bool writeOBJ(const Mesh& mesh, const std::string& path){
    std::ofstream file(path);
    if (!file){
        return false;
    }

    //sommets : un par ligne 
    for (size_t i = 0; i < mesh.positions.size(); i++) {
        const Vec3& p = mesh.positions[i];
        const Vec3& c = mesh.colors[i];
        file << "v " << p.x << " " << p.y << " " << p.z << " " << c.x << " " << c.y << " " << c.z << "\n";
    }

    //normals
    for (const Vec3& n : mesh.normals) {
        file << "vn " << n.x << " " << n.y << " " << n.z << "\n";
    }

    //faces
    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        //+1 : les indices OBJ commencent par 1
        int a = mesh.indices[i] + 1;
        int b = mesh.indices[i + 1] + 1;
        int c = mesh.indices[i + 2] + 1;

        //double slash veut dire "pas de coordonnee de texture"
        file << "f " << a << "//" << a << " "  << b << "//" << b << " "  << c << "//" << c << "\n";
    }

    return file.good();
}