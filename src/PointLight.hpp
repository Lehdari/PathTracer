#ifndef POINTLIGHT_HPP
#define POINTLIGHT_HPP


#include "Light.hpp"


class PointLight : public Light {
public:
    PointLight(const Vector3f& position, const Vector3f& color);

    LightSample drawSample(const Vector3f& p) const;

private:
    Vector3f position_;
    Vector3f color_;
};


#endif  //  POINTLIGHT_HPP
