#include "EventFunctions.hpp"

#include <iostream>


void moveCamera(BasicCamera& camera, bool local, const Vector3f& dir) {
    if (local)
        camera.moveLocal(dir);
    //else
        //camera.moveGlobal(dir);
}

void render(Renderer& renderer, BasicCamera& camera, Scene& scene,
            Light* light, Canvas& canvas,
            std::default_random_engine& r, bool& showRender,
            Filter& filter) {
    renderer.render(camera, scene, light, canvas, r);
    if (!showRender)
        showRender = true;

    //if (filter.getWidth() > 0.85f)
    //    filter.setWidth(filter.getWidth()*0.8f);
    //std::cout << "filter width: " << filter.getWidth() << std::endl;
}

void toggleShowRender(bool& showRender) {
    showRender = !showRender;
}

void saveRenderToFile(Canvas& canvas, const std::string& fileName) {
    canvas.saveToFile(fileName);
}
