#include "BasicCamera.hpp"

#include <iostream> //  TEMP


BasicCamera::BasicCamera(unsigned viewWidth, unsigned viewHeight,float fov, float aspectRatio) :
    viewWidth_(viewWidth), viewHeight_(viewHeight),
    fov_(fov), fovRatio_(tan(fov_*0.5)), aspectRatio_(aspectRatio)
{}

BasicCamera::BasicCamera(unsigned viewWidth, unsigned viewHeight, float fov) :
    BasicCamera(viewWidth, viewHeight, fov, (float)viewWidth/(float)viewHeight)
{}

void BasicCamera::lookAt(const Vector3f& from, const Vector3f& to, const Vector3f& up) {
    Vector3f xAxis, yAxis, zAxis;

    position_ = from;
    forward_ = (to - from).normalized();
    right_ = up.cross(forward_).normalized();
    up_ = forward_.cross(right_);

    orientation_ << right_[0]       , right_[1]     , right_[2]     , -right_.dot(from),
                    up_[0]          , up_[1]        , up_[2]        , -up_.dot(from),
                    -forward_[0]    , -forward_[1]  , -forward_[2]  , forward_.dot(from) ,
                    0.0f            , 0.0f          , 0.0f          , 1.0f;
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

void BasicCamera::render(Scene& scene) {
    Ray ray;

    render_.create(viewWidth_, viewHeight_);

    for (auto y=0u; y<viewHeight_; ++y) {
        for (auto x=0u; x<viewWidth_; ++x) {

            ray = generateRay(x + 0.5f, y + 0.5f);

            Hit hit = scene.traceRay(ray);

            if (hit.triangle)
                render_.setPixel(viewWidth_-x-1, viewHeight_-y-1, sf::Color(ray.t * 10, ray.t * 10, ray.t * 10));
        }
        printf("%0.3f%%\r", (float)(y*100)/viewHeight_);
    }

    render_.saveToFile("render.png");
}

Ray BasicCamera::generateRay(float x, float y) const {
    Ray out;

    float sx = (x/viewWidth_ - 0.5f) * fovRatio_;
    float sy = (y/viewHeight_ - 0.5f) * (fovRatio_/aspectRatio_);

    out.o = position_;
    out.d = (forward_ + sx*right_ + sy*up_).normalized();
    out.t = FLT_MAX;

    return out;
}
