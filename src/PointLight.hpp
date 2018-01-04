#ifndef POINTLIGHT_HPP
#define POINTLIGHT_HPP


#include "Light.hpp"

#include <mutex>


class PointLight : public Light {
public:
    PointLight(const Vector3f& position, const Vector3d& color);

    LightSample drawSample(const Vector3f& p) const;

private:
    Vector3f position_;
    Vector3d color_;
};


#endif  //  POINTLIGHT_HPP
