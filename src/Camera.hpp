#ifndef CAMERA_HPP
#define CAMERA_HPP


#include "Scene.hpp"
#include "Light.hpp"


template<typename T_Derived>
class Camera {
public:
    Ray generateRay(float x, float y, unsigned viewWidth, unsigned viewHeight) const {
        return static_cast<const T_Derived*>(this)->generateRay(x, y, viewWidth, viewHeight);
    }
};


#endif  //  CAMERA_HPP

