#include "Renderer.hpp"

#include <iostream> //TEMP


namespace { //  some helper stuff

    Matrix3f formBasis(const Vector3f& v) {
        //  form a vector which is not perpendicular to v
        Vector3f q = v;
        int min = 0;
        if (abs(v[1]) < abs(v[min])) min = 1;
        if (abs(v[2]) < abs(v[min])) min = 2;
        q[min] = 1.0f;

        Vector3f t = q.cross(v).normalized();
        Vector3f b = v.cross(t).normalized();
        Vector3f n = v.normalized();

        Matrix3f out;
        out <<  t, b, n;

        return out;
    }

    Vector3f cosineSample(std::default_random_engine& r) {
        float x = ((r() % 65536) / 32767.5f) - 1.0f;
        float y = ((r() % 65536) / 32767.5f) - 1.0f;
        float a = x*x + y*y;

        while (a >= 1) {
            x = ((r() % 65536) / 32767.5f) - 1.0f;
            y = ((r() % 65536) / 32767.5f) - 1.0f;
            a = x*x + y*y;
        }

        return Vector3f{x, y, std::sqrt(1 - a)};
    }
}

Renderer::Renderer(void)
{}

Renderer::~Renderer(void) {
}

Vector3d Renderer::bounce(Scene& scene, Ray& ray,
                          std::default_random_engine& r, unsigned nBounces) const {
    Vector3d lightOut{0.0, 0.0, 0.0};
    Hit hit = scene.traceRay(ray);

    if (hit.triangle()) {
        Vertex vh = hit.getBarycentric();

        //  BRDF
        Vector3f leaving, incoming;
        double incCos;  // cosine of ange between surface normal and incoming ray

        if (nBounces > 0) { //  recursive bounce
            leaving = -ray.d;

            ray.d = formBasis(vh.n)*cosineSample(r);    //  cosine sampling for now
            ray.o = vh.p + ray.d*RAY_EPS;
            ray.t = FLT_MAX;

            incoming = -ray.d;

            incCos = vh.n.dot(-incoming);

            if (incCos > 0.0f)
                lightOut = /*incCos **/ PI * bounce(scene, ray, r, nBounces-1); // no angle cosine required when using cosine distribution
        }

        //  Shadow rays
        auto& lights = scene.getLights();
        for (auto& l : lights) {
            LightSample ls = l->drawSample(vh.p);

            float lt = ls.ray.t;
            scene.traceRay(ls.ray);

            if (ls.ray.t >= lt - RAY_EPS) {
                float a = 1.0f / (ls.ray.t*ls.ray.t);

                incCos = vh.n.dot(-ls.ray.d);
                if (incCos > 0.0f)
                    lightOut += incCos * a * ls.col; //Vector3d{a*ls.col[0], a*ls.col[1], a*ls.col[2]};
            }
        }
    }

    return lightOut;
}
