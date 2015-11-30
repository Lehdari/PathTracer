#ifndef BASICCAMERA_HPP
#define BASICCAMERA_HPP


#include "Camera.hpp"
#include "MathTypes.hpp"

#include <SFML/Graphics.hpp>


class BasicCamera : public Camera<BasicCamera> {
public:
    BasicCamera(unsigned viewWidth, unsigned viewHeight, float fov, float aspectRatio);
    BasicCamera(unsigned viewWidth, unsigned viewHeight, float fov);

    void lookAt(const Vector3f& from, const Vector3f& to, const Vector3f& up);

    const Vector3f& getPosition(void) const;
    const Vector3f& getForward(void) const;
    const Vector3f& getRight(void) const;
    const Vector3f& getUp(void) const;

    void render(Scene& scene, Light* light);

protected:
    Ray generateRay(float x, float y) const;

private:
    unsigned viewWidth_;    //  View width in pixels
    unsigned viewHeight_;   //  View height in pixels

    float fov_;
    float fovRatio_;
    float aspectRatio_;

    Matrix4f orientation_;
    Vector3f position_;
    Vector3f forward_;
    Vector3f right_;
    Vector3f up_;

    sf::Image render_;
};


#endif  //  BASICCAMERA_HPP

