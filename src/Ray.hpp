#ifndef RAY_HPP
#define RAY_HPP


#include "MathTypes.hpp"


struct Ray {
    Vector3f o; //  ray origin
    Vector3f d; //  ray direction (normalized!)
    float t;    //  ray length (why d has to be normalized)
};


#endif  //  RAY_HPP
