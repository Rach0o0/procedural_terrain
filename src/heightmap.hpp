#pragma once
#include <cstddef>
#include <vector>

class Heightmap {
public: 
    Heightmap(size_t width, size_t height)
        : width_(width), height_(height), values_(width*height, 0.0f){}

    //get width and height
    size_t width() const {return width_;}
    size_t height() const {return height_;}

    //write
    float& at(size_t x, size_t y) { return values_[y * width_ + x];}
    //read
    float at(size_t x, size_t y) const { return values_[y * width_ + x];}

    //list of values
    const std::vector<float>& values() const { return values_;}

private:
    size_t width_;
    size_t height_;
    std::vector<float> values_;
};