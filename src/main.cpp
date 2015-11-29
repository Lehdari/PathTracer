#include "Scene.hpp"
#include "BasicCamera.hpp"

#include <iostream>


int main(void) {
    /*std::vector<Triangle> triangles;
    triangles.push_back(Triangle{ Vector3f{-1.0f, -1.0f,  0.0f},
                                  Vector3f{1.0f,  -1.0f,  0.0f},
                                  Vector3f{-1.0f, 1.0f,   0.0f} });
    triangles.push_back(Triangle{ Vector3f{-1.0f, -1.0f,  1.0f},
                                  Vector3f{1.0f,  -1.0f,  1.0f},
                                  Vector3f{-1.0f, 1.0f,   1.0f} });*/
    Scene scene;
    scene.loadFromObj("res/scenes/torus.obj");

    BasicCamera camera(400, 300, 90.0, 4.0/3.0);

    camera.lookAt( {2.0f, 3.0f, 4.0f},
                   {0.0f, 0.0f, 0.0f},
                   {0.0f, 1.0f, 0.0f} );

    camera.render(scene);

    return 0;
}
