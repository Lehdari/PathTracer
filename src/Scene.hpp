#ifndef SCENE_HPP
#define SCENE_HPP


#include "Triangle.hpp"
#include "Ray.hpp"
#include "Hit.hpp"
//#include "Light.hpp"

#include <vector>
#include <memory>


class Scene {
public:
    Scene(void) {}
    Scene(const std::vector<Triangle>& triangles);

    void loadFromObj(const std::string& fileName);

    //void addLight(Light* light);

    Hit traceRay(Ray& ray) const;

    const std::vector<Triangle>& getTriangles(void) const;

private:
    //  triangle data
    std::vector<Triangle> triangles_;
    std::vector<Vertex> vertices_;

    bool usingTexCoords_;
    bool usingNormals_;
    bool usingIndexing_;

    //  light data
    //std::vector<std::unique_ptr<Light>> lights_;

    Hit intersectRay(Ray& ray, const Triangle& triangle) const;
};


#endif  //  SCENE_HPP
