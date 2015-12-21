#include "EventFunctions.hpp"


void moveCamera(BasicCamera& camera, bool local, const Vector3f& dir) {
    if (local)
        camera.moveLocal(dir);
    //else
        //camera.moveGlobal(dir);
}

void render(Renderer& renderer, BasicCamera& camera, Scene& scene,
            Light* light, Canvas& canvas,
            std::default_random_engine& r, bool& showRender) {
    renderer.render(camera, scene, light, canvas, r);
    if (!showRender)
        showRender = true;
}
