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
                          std::default_random_engine& r,
                          unsigned maxBounces, unsigned bounceId) const {
    Vector3d lightOut{0.0, 0.0, 0.0};
    Hit hit = scene.traceRay(ray);

    if (hit.triangle()) {
        Vertex vh = hit.getBarycentric();

        Vector3f leaving, incoming;
        double cosTheta;  // cosine of ange between surface normal and incoming ray
        double p;
        Vector3d brdf(1.0/PI, 1.0/PI, 1.0/PI);

        //  Shadow ray
        auto& lights = scene.getLights();
        auto nl = lights.size();
        auto l = lights[r()%nl].get();
        LightSample ls = l->drawSample(vh.p);

        float lt = ls.ray.t;
        scene.traceRay(ls.ray);

        double tProb = 0.12;//0.01*bounceId;

        if (ls.ray.t >= lt-RAY_EPS) {
            float a = 1.0f/(ls.ray.t*ls.ray.t);

            cosTheta = vh.n.dot(-ls.ray.d);
            if (cosTheta > 0.0f) {
                lightOut += cosTheta*brdf.cwiseProduct(a*ls.col)/nl;
                tProb = 0.01;
            }
        }

        //  reflection
        //double tProb = 1.0-cosTheta;//0.5;
        bool terminate = (r()%65536)*0.000015259021896696 < tProb;

        if (!terminate && bounceId <= maxBounces) { //  recursive bounce
            leaving = -ray.d;

            ray.d = formBasis(vh.n)*cosineSample(r);    //  cosine sampling for now
            ray.o = vh.p + ray.d*RAY_EPS;
            ray.t = FLT_MAX;

            incoming = -ray.d;

            cosTheta = vh.n.dot(-incoming);
            p = cosTheta/PI;

            if (cosTheta > 0.0f)
                lightOut += 0.9 * cosTheta * brdf.cwiseProduct(bounce(scene, ray, r, maxBounces, bounceId+1)) / p / (1.0f-tProb);
        }
    }

    return lightOut;
}
