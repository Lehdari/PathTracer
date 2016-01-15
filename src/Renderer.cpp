#include "Renderer.hpp"


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

        while (x*x + y*y > 1) {
            x = ((r() % 65536) / 32767.5f) - 1.0f;
            y = ((r() % 65536) / 32767.5f) - 1.0f;
        }

        return Vector3f{x, y, sqrtf(1 - x*x - y*y)};
    }
}

Renderer::Renderer(unsigned maxThreads) :
    maxThreads_(maxThreads),
    threadsRunning_(0),
    castedPaths_(0ull)
{}

Renderer::~Renderer(void) {
    for (auto& thread : threads_)
        if (thread.joinable())
            thread.join();
}

Vector3d Renderer::bounce(Scene& scene, Light* light, Ray& ray,
                             std::default_random_engine& r, unsigned nBounces) const {
    Vector3d lightOut{0.0, 0.0, 0.0};
    Hit hit = scene.traceRay(ray);

    if (hit.triangle()) {
        Vertex vh = hit.getBarycentric();

        //  BRDF
        Vector3f leaving, incoming;
        float brdf;

        if (nBounces > 0) { //  recursive bounce
            leaving = -ray.d;

            ray.d = formBasis(vh.n)*cosineSample(r);    //  cosine sampling for now
            ray.o = vh.p + ray.d*RAY_EPS;
            ray.t = FLT_MAX;

            incoming = -ray.d;

            brdf = vh.n.dot(-incoming);
            if (brdf < 0.0f) brdf = 0.0f;

            lightOut = brdf * bounce(scene, light, ray, r, nBounces-1);
        }

        //  Shadow ray
        LightSample ls = light->drawSample(vh.p);
        float lt = ls.ray.t;

        scene.traceRay(ls.ray);

        if (ls.ray.t >= lt - RAY_EPS) {
            float a = 1.0f / (ls.ray.t*ls.ray.t);

            brdf = vh.n.dot(-ls.ray.d);
            if (brdf < 0.0f) brdf = 0.0f;

            lightOut += brdf * Vector3d{a*ls.col[0], a*ls.col[1], a*ls.col[2]};
        }
    }

    return lightOut;
}
