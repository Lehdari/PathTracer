#ifndef SCENE_HPP
#define SCENE_HPP


#include "Triangle.hpp"
#include "Ray.hpp"
#include "Hit.hpp"

#include <vector>
#include <mutex>


class Scene {
public:
    Scene(void) {}
    Scene(const std::vector<Triangle>& triangles);

    void loadFromObj(const std::string& fileName);

    //void addLight(Light* light);

    Hit traceRay(Ray& ray);

    const std::vector<Triangle>& getTriangles(void);

private:
    //  triangle data
    std::vector<Triangle> triangles_;
    std::vector<Vertex> vertices_;
    std::mutex triangleMutex_;

    bool usingTexCoords_;
    bool usingNormals_;
    bool usingIndexing_;

    Hit intersectRay(Ray& ray, const Triangle& triangle) const;
};


#endif  //  SCENE_HPP
