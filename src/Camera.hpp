#ifndef CAMERA_HPP
#define CAMERA_HPP


#include "Scene.hpp"


template<typename T_Derived>
class Camera {
public:
    void render(Scene& scene) {
        static_cast<T_Derived>(this)->render(scene);
    }
};


#endif  //  CAMERA_HPP

