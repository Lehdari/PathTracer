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

    void loadFromObj(const std::string& fileName);

    const std::vector<Triangle>& getTriangles(void) const;

    Hit traceRay(Ray& ray) const;

private:
    std::vector<Triangle> triangles_;
    std::vector<Vertex> vertices_;

    bool usingTexCoords_;
    bool usingNormals_;
    bool usingIndexing_;

    Hit intersectRay(Ray& ray, const Triangle& triangle) const;
};


#endif  //  SCENE_HPP
