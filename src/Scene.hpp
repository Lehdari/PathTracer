#ifndef SCENE_HPP
#define SCENE_HPP


#include "Triangle.hpp"
#include "Ray.hpp"
#include "Hit.hpp"
#include "Bvh.hpp"
#include "Light.hpp"

#include <vector>
#include <mutex>
#include <memory>


class Scene {
public:
    Scene(void);
    Scene(const std::vector<Triangle>& triangles);

    void loadFromObj(const std::string& fileName);

    template<typename T_Light>
    void addLight(T_Light&& light);

    Hit traceRay(Ray& ray);

    const std::vector<Triangle>& getTriangles(void);
    const std::vector<std::unique_ptr<Light>>&   getLights(void);

private:
    //  triangle data
    std::vector<Triangle> triangles_;
    std::vector<Vertex> vertices_;
    std::mutex triangleMutex_;

    bool usingTexCoords_;
    bool usingNormals_;
    bool usingIndexing_;

    //  BVH
    Bvh bvh_;

    //  light data
    std::vector<std::unique_ptr<Light>>  lights_;


    Hit intersectRay(Ray& ray, const Triangle& triangle) const;
};


template<typename T_Light>
void Scene::addLight(T_Light&& light)
{
    lights_.emplace_back(new T_Light(light));
}


#endif  //  SCENE_HPP
