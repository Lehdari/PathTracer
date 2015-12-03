#include "BasicCamera.hpp"

#include <iostream> //  TEMP


BasicCamera::BasicCamera(unsigned viewWidth, unsigned viewHeight,
                         float fov, float aspectRatio, float near, float far) :
    viewWidth_(viewWidth), viewHeight_(viewHeight),
    fov_(fov), fovRatio_(tan(fov_*0.5f)), aspectRatio_(aspectRatio),
    orientation_(Matrix4f::Identity())
{
    projection(fov_, aspectRatio_, near, far);
}

BasicCamera::BasicCamera(unsigned viewWidth, unsigned viewHeight,
                         float fov, float near, float far) :
    BasicCamera(viewWidth, viewHeight, fov, (float)viewWidth/(float)viewHeight, near, far)
{}

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

void BasicCamera::render(Scene& scene, Light* light, const std::string& fileName) {
    Ray ray;

    render_.create(viewWidth_, viewHeight_);

    for (auto y=0u; y<viewHeight_; ++y) {
        for (auto x=0u; x<viewWidth_; ++x) {
            ray = generateRay(x + 0.5f, y + 0.5f);

            Hit hit = scene.traceRay(ray);

            if (hit.triangle()) {

                Vertex vh = hit.getBarycentric();

                LightSample ls = light->drawSample(vh.p);
                float lt = ls.ray.t;

                Hit shadowHit = scene.traceRay(ls.ray);

                if (ls.ray.t >= lt - 0.00001) {

                    //render_.setPixel(viewWidth_-x-1, viewHeight_-y-1, sf::Color(ray.t * 10, ray.t * 10, ray.t * 10));
                    /*render_.setPixel(viewWidth_-x-1, viewHeight_-y-1, sf::Color(vh.n[0] * 127 + 128,
                                                                                vh.n[1] * 127 + 128,
                                                                                vh.n[2] * 127 + 128));*/
                    float a = 1.0f / (ls.ray.t*ls.ray.t);
                    render_.setPixel(x, viewHeight_-y-1, sf::Color(a * 40,
                                                                                a * 40,
                                                                                a * 40));
                }
            }
        }
        printf("%0.3f%%\r", (float)(y*100)/viewHeight_);
    }
    printf("\n");
    render_.saveToFile(fileName);
}

Ray BasicCamera::generateRay(float x, float y) const {
    Ray out;

    float sx = ((x/viewWidth_)*2.0f - 1.0f) * fovRatio_;
    float sy = ((y/viewHeight_)*2.0f - 1.0f) * (fovRatio_/aspectRatio_);

    out.o = position_;
    out.d = (forward_ + sx*right_ + sy*up_).normalized();
    out.t = FLT_MAX;

    return out;
}
