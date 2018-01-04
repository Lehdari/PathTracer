#ifndef RENDERER_HPP
#define RENDERER_HPP


#include "Scene.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Canvas.hpp"
#include "Sampler.hpp"

#include <vector>

#define RAY_EPS 0.00001f


class Renderer {
public:
    Renderer(void);
    ~Renderer(void);

    Renderer(const Renderer& other)             = delete;
    Renderer(Renderer&& other)                  = delete;
    Renderer& operator=(const Renderer& other)  = delete;
    Renderer& operator=(Renderer&& other)       = delete;

    template<typename T_Camera>
    void render(Camera<T_Camera>& camera, Scene& scene, Canvas& canvas,
                std::default_random_engine& r);

protected:
    template<typename T_Camera>
    void renderPatch(Camera<T_Camera>& camera, Scene& scene, Canvas& canvas,
                     std::default_random_engine& r,
                     unsigned xMin, unsigned xMax, unsigned yMin, unsigned yMax);
    Vector3d bounce(Scene& scene, Ray& ray,
                    std::default_random_engine& r, unsigned nBounces = 0) const;
};


template<typename T_Camera>
void Renderer::render(Camera<T_Camera>& camera, Scene& scene, Canvas& canvas,
                std::default_random_engine& r) {
    unsigned viewW = canvas.getWidth();
    unsigned viewH = canvas.getHeight();

    printf("Rendering..\n");
    unsigned n = 0;
    #pragma omp parallel for
    for (auto y=0u; y<viewH; ++y) {
        renderPatch(camera, scene, canvas, r, 0, viewW, y, y+1);

        #pragma omp critical
        {
            ++n;
            printf("%u/%u\r", n, viewH);
        }
    }
}

/*template<typename T_Camera>
void Renderer::dispatchThread(Camera<T_Camera>& camera, Scene& scene, Light* light, Canvas& canvas,
                              std::default_random_engine& r,
                              unsigned xMin, unsigned xMax, unsigned yMin, unsigned yMax) {
    std::lock_guard<std::mutex> lock(threadsMutex_);
    if (threads_.size() < maxThreads_) {
        threads_.emplace_back([&] { renderAsync(threadRunning_[threads_.size()],
                                                camera, scene, light, canvas, r,
                                                xMin, xMax, yMin, yMax); } );

    }
    else {
        for (auto i=0u; i<maxThreads_; ++i) {
            if (!threadRunning_[i]) {
                if (threads_[i].joinable()) threads_[i].join();
                threads_[i] = std::thread([&] { renderAsync(threadRunning_[threads_.size()],
                                                            camera, scene, light, canvas, r,
                                                            xMin, xMax, yMin, yMax); } );
            }
        }
    }
}*/

template<typename T_Camera>
void Renderer::renderPatch(Camera<T_Camera>& camera, Scene& scene, Canvas& canvas,
                           std::default_random_engine& r,
                           unsigned xMin, unsigned xMax, unsigned yMin, unsigned yMax) {
    unsigned viewW = canvas.getWidth();
    unsigned viewH = canvas.getHeight();

    const unsigned nSamples = 16;
    Sampler sampler(Sampler::TYPE_JITTERED, nSamples);
    Ray ray;

    for (auto y=yMin; y<yMax; ++y) {
        for (auto x=xMin; x<xMax; ++x) {
            for (auto i=0u; i<nSamples; ++i) {
                float rayX, rayY;
                sampler.drawSample(rayX, rayY, i, r);
                rayX += x;
                rayY += y;
                ray = camera.generateRay(rayX, rayY, viewW, viewH);
                Vector3d pathLight = bounce(scene, ray, r, 4);
                Sample s({rayX, rayY},
                         {pathLight[0], pathLight[1], pathLight[2]});
                canvas.addSample(s);
            }
        }
        //printf("%0.3f%%\r", (float)((y-yMin)*100)/(yMax-yMin));
    }
    //printf("\n");
}


#endif  //  RENDERER_HPP
