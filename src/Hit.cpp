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
