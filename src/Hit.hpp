#ifndef HIT_HPP
#define HIT_HPP


#include "Triangle.hpp"
#include "Ray.hpp"


class Hit {
public:
    Hit(void);

    void set(const Triangle* triangle, float beta, float gamma);
    void set(float beta, float gamma);

    const Triangle* triangle(void) const;
    float beta(void) const;
    float gamma(void) const;

    const Vertex& getBarycentric(void) const;


    static Hit intersectRay(Ray& ray, const Triangle& triangle);

private:
    const Triangle* triangle_;
    float beta_;
    float gamma_;

    //  Barycentric vertex calculated from triangle vertices
    Vertex barycentric_;
};




#endif  // HIT_HPP
