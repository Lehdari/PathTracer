#ifndef RAY_HPP
#define RAY_HPP


#include "MathTypes.hpp"


struct Ray {
    Vector3f o; //  ray origin
    Vector3f d; //  ray direction
    float t;    //  ray length
};


#endif  //  RAY_HPP
