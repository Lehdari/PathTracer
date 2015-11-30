#ifndef CAMERA_HPP
#define CAMERA_HPP


#include "Scene.hpp"
#include "Light.hpp"


template<typename T_Derived>
class Camera {
public:
    void render(Scene& scene, Light* light) {
        static_cast<T_Derived>(this)->render(scene, light);
    }
};


#endif  //  CAMERA_HPP

