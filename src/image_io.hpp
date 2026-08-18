#pragma once

#include <string> 
#include "heightmap.hpp"

bool writeGrayscalePng(const Heightmap& map, const std::string& path, bool normalize = true);
