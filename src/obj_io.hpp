#pragma once

#include <string>
#include "mesh.hpp"

//ecrit le mesh au format OBJ
bool writeOBJ(const Mesh& mesh, const std::string& path);