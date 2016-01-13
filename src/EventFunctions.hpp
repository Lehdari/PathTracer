#ifndef EVENTFUNCTIONS_HPP
#define EVENTFUNCTIONS_HPP


#include "BasicCamera.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Light.hpp"
#include "Canvas.hpp"

#include <random>


void moveCamera(BasicCamera& camera, bool local, const Vector3f& dir);
void render(Renderer& renderer, BasicCamera& camera, Scene& scene,
            Light* light, Canvas& canvas,
            std::default_random_engine& r, bool& showRender,
            Filter& filter);
void toggleShowRender(bool& showRender);
void saveRenderToFile(Canvas& canvas, const std::string& fileName);


#endif // EVENTFUNCTIONS_HPP
