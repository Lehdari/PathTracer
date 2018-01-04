#ifndef LIGHT_HPP
#define LIGHT_HPP


#include "Ray.hpp"


struct LightSample {
    Ray ray;        //  ray from light
    Vector3d col;   //  color(and brightness) of the light
};


class Light {
public:
    //  draw sample from given point
    virtual LightSample drawSample(const Vector3f& p) const = 0;
};


#endif  //  LIGHT_HPP
