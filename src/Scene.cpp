#include "Scene.hpp"


#include <iostream> //  TEMP


Scene::Scene(const std::vector<Triangle>& triangles) :
    triangles_(triangles)
{}

const std::vector<Triangle>& Scene::getTriangles(void) const {
    return triangles_;
}

Hit Scene::traceRay(Ray& ray) const {
    Hit hit{ nullptr, 0.0f, 0.0f };

    for (auto& tri : triangles_) {
        Hit newHit = intersectRay(ray, tri);

        if (newHit.triangle)
            hit = newHit;
    }
}

Hit Scene::intersectRay(Ray& ray, const Triangle& triangle) const {
    Hit hit;
    hit.triangle = nullptr;
    hit.beta = 0.0f;
    hit.gamma = 0.0f;

    float axrox = triangle.p[0][0] - ray.o[0];
    float ayroy = triangle.p[0][1] - ray.o[1];
    float azroz = triangle.p[0][2] - ray.o[2];

    float axbx = triangle.p[0][0] - triangle.p[1][0];
    float ayby = triangle.p[0][1] - triangle.p[1][1];
    float azbz = triangle.p[0][2] - triangle.p[1][2];

    float axcx = triangle.p[0][0] - triangle.p[2][0];
    float aycy = triangle.p[0][1] - triangle.p[2][1];
    float azcz = triangle.p[0][2] - triangle.p[2][2];

    Matrix3f mA;
    mA <<       axbx,   axcx,   ray.d[0],
                ayby,   aycy,   ray.d[1],
                azbz,   azcz,   ray.d[2];
    float aDet = mA.determinant();

    Matrix3f mBeta;
    mBeta <<    axrox,  axcx,   ray.d[0],
                ayroy,  aycy,   ray.d[1],
                azroz,  azcz,   ray.d[2];

    Matrix3f mGamma;
    mGamma <<   axbx,   axrox,  ray.d[0],
                ayby,   ayroy,  ray.d[1],
                azbz,   azroz,  ray.d[2];

    float beta =  mBeta.determinant() / aDet;
    float gamma = mGamma.determinant() / aDet;

    if (beta > 0.0f && gamma > 0.0f && beta+gamma < 1.0f) {
        Matrix3f mT;
        mT <<       axbx,   axcx,   axrox,
                    ayby,   aycy,   ayroy,
                    azbz,   azcz,   azroz;
        float t = mT.determinant() / aDet;

        if (t < ray.t) {
            ray.t = t;
            hit.triangle = &triangle;
            hit.beta = beta;
            hit.gamma = gamma;
        }
    }

    return hit;
}
