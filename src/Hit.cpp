#include "Hit.hpp"


Hit::Hit(void) :
    triangle_(nullptr), beta_(0.0f), gamma_(0.0f)
{}

void Hit::set(const Triangle* triangle, float beta, float gamma) {
    triangle_ = triangle;
    set(beta, gamma);
}

void Hit::set(float beta, float gamma) {
    if (triangle_) {
        barycentric_.p = triangle_->v[0]->p +
                         beta*(triangle_->v[1]->p-triangle_->v[0]->p) +
                         gamma*(triangle_->v[2]->p-triangle_->v[0]->p);
        barycentric_.t = triangle_->v[0]->t +
                         beta*(triangle_->v[1]->t-triangle_->v[0]->t) +
                         gamma*(triangle_->v[2]->t-triangle_->v[0]->t);
        barycentric_.n = triangle_->v[0]->n +
                         beta*(triangle_->v[1]->n-triangle_->v[0]->n) +
                         gamma*(triangle_->v[2]->n-triangle_->v[0]->n);
    }
}

const Triangle* Hit::triangle(void) const {
    return triangle_;
}

float Hit::beta(void) const {
    return beta_;
}

float Hit::gamma(void) const {
    return gamma_;
}

const Vertex& Hit::getBarycentric(void) const {
    return barycentric_;
}

Hit Hit::intersectRay(Ray& ray, const Triangle& triangle) {
    Hit hit;

    float axrox = triangle.v[0]->p[0] - ray.o[0];
    float ayroy = triangle.v[0]->p[1] - ray.o[1];
    float azroz = triangle.v[0]->p[2] - ray.o[2];

    float axbx = triangle.v[0]->p[0] - triangle.v[1]->p[0];
    float ayby = triangle.v[0]->p[1] - triangle.v[1]->p[1];
    float azbz = triangle.v[0]->p[2] - triangle.v[1]->p[2];

    float axcx = triangle.v[0]->p[0] - triangle.v[2]->p[0];
    float aycy = triangle.v[0]->p[1] - triangle.v[2]->p[1];
    float azcz = triangle.v[0]->p[2] - triangle.v[2]->p[2];

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

        if (t > 0.0f && t < ray.t) {
            ray.t = t;
            hit.set(&triangle, beta, gamma);
        }
    }

    return hit;
}
