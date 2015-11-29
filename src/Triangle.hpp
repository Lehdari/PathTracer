#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP


#include "MathTypes.hpp"


struct Vertex {
    Vector3f p; //  Position
    Vector2f t; //  Texture Coordinate
    Vector3f n; //  Normal
};

struct Triangle {
    Vertex* v[3];
};


#endif  //  TRIANGLE_HPP
