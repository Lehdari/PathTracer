#ifndef SCENE_HPP
#define SCENE_HPP


#include "Triangle.hpp"
#include "Ray.hpp"
#include "Hit.hpp"

#include <vector>


class Scene {
public:
    Scene(void) {}
    Scene(const std::vector<Triangle>& triangles);

    const std::vector<Triangle>& getTriangles(void) const;

    Hit traceRay(Ray& ray) const;

private:
    std::vector<Triangle> triangles_;

    Hit intersectRay(Ray& ray, const Triangle& triangle) const;
};


#endif  //  SCENE_HPP
