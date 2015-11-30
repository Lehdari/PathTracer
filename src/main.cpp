#include "Scene.hpp"
#include "BasicCamera.hpp"
#include "PointLight.hpp"

#include <iostream>


int main(void) {
    //  Scene
    Scene scene;
    scene.loadFromObj("res/scenes/cornell.obj");

    //  Light
    PointLight light({0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f});
    //scene.addLight(&light);

    //  Camera
    BasicCamera camera(400, 300, 90.0, 4.0/3.0);
    camera.lookAt( {-0.7f, -0.8f, 0.95f},
                   {0.0f, -0.5f, 0.0f},
                   {0.0f, 1.0f, 0.0f} );

    //  Render
    camera.render(scene, &light);

    return 0;
}
