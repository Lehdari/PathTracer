#ifndef BASICCAMERA_HPP
#define BASICCAMERA_HPP


#include "Camera.hpp"
#include "MathTypes.hpp"
#include "Canvas.hpp"


class BasicCamera : public Camera<BasicCamera> {
public:
    BasicCamera(float fov, float aspectRatio, float near, float far);

    void lookAt(const Vector3f& from, const Vector3f& to, const Vector3f& up);
    void projection(float fov, float aspectRatio, float near, float far);

    void moveLocal(const Vector3f& m);

    const Vector3f& getPosition(void) const;
    const Vector3f& getForward(void) const;
    const Vector3f& getRight(void) const;
    const Vector3f& getUp(void) const;

    Matrix4f getVP(void) const;

    void render(Scene& scene, Light* light, Canvas& canvas);

protected:
    Ray generateRay(float x, float y, unsigned viewWidth, unsigned viewHeight) const;

private:
    float fov_;
    float fovRatio_;
    float aspectRatio_;

    Matrix4f orientation_;
    Matrix4f projection_;

    Vector3f position_;
    Vector3f forward_;
    Vector3f right_;
    Vector3f up_;
};


#endif  //  BASICCAMERA_HPP

