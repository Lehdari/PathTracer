#ifndef HIT_HPP
#define HIT_HPP


#include "Triangle.hpp"


struct Hit {
    const Triangle* triangle;
    float beta;
    float gamma;
};


#endif  // HIT_HPP
