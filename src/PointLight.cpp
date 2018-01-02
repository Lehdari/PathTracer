#include "PointLight.hpp"


PointLight::PointLight(const Vector3f& position, const Vector3d& color) :
    position_(position), color_(color)
{}

LightSample PointLight::drawSample(const Vector3f& p) {
    Vector3f pd = p-position_;

    LightSample s;
    s.ray.o = position_;
    s.ray.d = pd.normalized();
    s.ray.t = pd.norm();
    s.col = color_;

    return s;
}
