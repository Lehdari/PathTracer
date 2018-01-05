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
        double brdf;
        //double prob = 1.0f / PI;
        double invProb = PI;

        if (nBounces > 0) { //  recursive bounce
            leaving = -ray.d;

            auto m = formBasis(vh.n);
            auto cs = cosineSample(r);

            ray.d = m*cs;    //  cosine sampling for now
            ray.o = vh.p + ray.d*RAY_EPS;
            ray.t = FLT_MAX;

            incoming = -ray.d;

            brdf = vh.n.dot(-incoming);

            if (brdf != brdf) {
                printf("\nincoming: %0.2f %0.2f %0.2f\n", incoming[0], incoming[1], incoming[2]);
                std::cout << m;
                std::cout << cs;
            }

            if (brdf < 0.0f) brdf = 0.0f;

            lightOut = brdf * bounce(scene, ray, r, nBounces-1);
        }

        //  Shadow rays
        auto& lights = scene.getLights();
        for (auto& l : lights) {
            LightSample ls = l->drawSample(vh.p);

            float lt = ls.ray.t;
            //if (lt < RAY_EPS) lt = RAY_EPS;

            scene.traceRay(ls.ray);

            if (ls.ray.t >= lt - RAY_EPS) {
                float a = 1.0f / (ls.ray.t*ls.ray.t);
                //if (a != a)
                //    printf("\nasd\n");

                brdf = vh.n.dot(-ls.ray.d);
                if (brdf < 0.0f) brdf = 0.0f;

                lightOut += brdf * a * ls.col; //Vector3d{a*ls.col[0], a*ls.col[1], a*ls.col[2]};
            }
        }
    }

    return lightOut;
}
