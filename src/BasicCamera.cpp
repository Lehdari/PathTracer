#include "BasicCamera.hpp"

#include <iostream> //  TEMP


#define RAY_EPS 0.00001f


namespace {

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


BasicCamera::BasicCamera(float fov, float aspectRatio, float near, float far) :
    fov_(fov), fovRatio_(tan(fov_*0.5f)), aspectRatio_(aspectRatio),
    orientation_(Matrix4f::Identity())
{
    projection(fov_, aspectRatio_, near, far);
}

void BasicCamera::lookAt(const Vector3f& from, const Vector3f& to, const Vector3f& up) {
    position_ = from;
    forward_ = (to - from).normalized();
    right_ = up.cross(-forward_).normalized();
    up_ = -forward_.cross(right_);

    orientation_ << right_[0]       , right_[1]     , right_[2]     , -right_.dot(position_),
                    up_[0]          , up_[1]        , up_[2]        , -up_.dot(position_),
                    -forward_[0]    , -forward_[1]  , -forward_[2]  , forward_.dot(position_) ,
                    0.0f            , 0.0f          , 0.0f          , 1.0f;
}

void BasicCamera::projection(float fov, float aspectRatio, float near, float far) {
    float r = tanf(fov / 2.0f) * near;
    float t = r / aspectRatio;

    projection_ << near/r   , 0.0f      , 0.0f                  , 0.0f                      ,
                   0.0f     , near/t    , 0.0f                  , 0.0f                      ,
                   0.0f     , 0.0f      , -(far+near)/(far-near), -2.0f*far*near/(far-near) ,
                   0.0f     , 0.0f      , -1.0f                 , 0.0f                      ;
}

void BasicCamera::moveLocal(const Vector3f& m) {
    Matrix4f move;
    move <<   1.0f , 0.0f , 0.0f , m[0],
              0.0f , 1.0f , 0.0f , m[1],
              0.0f , 0.0f , 1.0f , m[2],
              0.0f , 0.0f , 0.0f , 0.0f;


    move = move*orientation_;
    Vector4f v;
    v << position_, 1.0f;

    Vector4f m2;
    m2 << m, 0.0f;

    position_ += (orientation_*m2).block<3,1>(0,0);

    //orientation_ = orientation_ * move;
    /*orientation_ << right_[0]       , right_[1]     , right_[2]     , -right_.dot(position_),
                    up_[0]          , up_[1]        , up_[2]        , -up_.dot(position_),
                    -forward_[0]    , -forward_[1]  , -forward_[2]  , forward_.dot(position_) ,
                    0.0f            , 0.0f          , 0.0f          , 1.0f;
    */
}

const Vector3f& BasicCamera::getPosition(void) const {
    return position_;
}

const Vector3f& BasicCamera::getForward(void) const {
    return forward_;
}

const Vector3f& BasicCamera::getRight(void) const {
    return right_;
}

const Vector3f& BasicCamera::getUp(void) const {
    return up_;
}

Matrix4f BasicCamera::getVP(void) const {
    return projection_ * orientation_;
}

void BasicCamera::render(Scene& scene, Light* light, Canvas& canvas, std::default_random_engine& r) {
    unsigned viewW = canvas.getWidth();
    unsigned viewH = canvas.getHeight();

    Ray ray;

    for (auto y=0u; y<viewH; ++y) {
        for (auto x=0u; x<viewW; ++x) {
            //printf("%u, %u: ", x, y);
            ray = generateRay(x + 0.5f, y + 0.5f, viewW, viewH);
            Vector3d pathLight = bounce(scene, light, ray, r, 1);

            canvas.addSample({x + 0.5f, viewH-y-1 + 0.5f},
                             {pathLight[0], pathLight[1], pathLight[2]});
        }
        printf("%0.3f%%\r", (float)(y*100)/viewH);
    }
    printf("\n");
}

Ray BasicCamera::generateRay(float x, float y, unsigned viewWidth, unsigned viewHeight) const {
    Ray out;

    float sx = ((x/viewWidth)*2.0f - 1.0f) * fovRatio_;
    float sy = ((y/viewHeight)*2.0f - 1.0f) * (fovRatio_/aspectRatio_);

    out.o = position_;
    out.d = (forward_ + sx*right_ + sy*up_).normalized();
    out.t = FLT_MAX;

    return out;
}

Vector3d BasicCamera::bounce(Scene& scene, Light* light, Ray& ray,
                             std::default_random_engine& r, unsigned nBounces) const {
    Vector3d lightOut{0.0, 0.0, 0.0};
    Hit hit = scene.traceRay(ray);

    if (hit.triangle()) {
        Vertex vh = hit.getBarycentric();

        //  BRDF
        Vector3f leaving, incoming;
        float brdf;

        if (nBounces > 0) { //  bounce
            leaving = -ray.d;

            ray.d = formBasis(vh.n)*cosineSample(r);    //  cosine sampling for now
            ray.o = vh.p + ray.d*RAY_EPS;
            ray.t = FLT_MAX;

            incoming = -ray.d;

            brdf = vh.n.dot(-incoming);
            if (brdf < 0.0f) brdf = 0.0f;

            lightOut = brdf * bounce(scene, light, ray, r, nBounces-1);
        }
        //else {  //  shadow ray
            LightSample ls = light->drawSample(vh.p);
            float lt = ls.ray.t;

            scene.traceRay(ls.ray);

            if (ls.ray.t >= lt - RAY_EPS) {
                float a = 1.0f / (ls.ray.t*ls.ray.t);

                brdf = vh.n.dot(-ls.ray.d);
                if (brdf < 0.0f) brdf = 0.0f;

                lightOut += brdf * Vector3d{a*ls.col[0], a*ls.col[1], a*ls.col[2]};
            }
        //}

        //lightOut *= brdf;
    }

    return lightOut;
}
