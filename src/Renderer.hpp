#ifndef RENDERER_HPP
#define RENDERER_HPP


#include "Scene.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Canvas.hpp"

#include <vector>
#include <thread>


#define RAY_EPS 0.00001f


class Renderer {
public:
    Renderer(unsigned maxThreads);
    ~Renderer(void);

    Renderer(const Renderer& other)             = delete;
    Renderer(Renderer&& other)                  = delete;
    Renderer& operator=(const Renderer& other)  = delete;
    Renderer& operator=(Renderer&& other)       = delete;

    template<typename T_Camera>
    void render(Camera<T_Camera>& camera, Scene& scene, Light* light, Canvas& canvas,
                std::default_random_engine& r);

protected:
    unsigned maxThreads_;
    std::vector<std::thread> threads_;
    std::vector<bool> threadRunning_;
    std::mutex threadsMutex_;

    /*template<typename T_Camera>
    void dispatchThread(Camera<T_Camera>& camera, Scene& scene, Light* light, Canvas& canvas,
                        std::default_random_engine& r,
                        unsigned xMin, unsigned xMax, unsigned yMin, unsigned yMax);*/

    template<typename T_Camera>
    void renderAsync(Camera<T_Camera>& camera, Scene& scene, Light* light, Canvas& canvas,
                     std::default_random_engine& r,
                     unsigned xMin, unsigned xMax, unsigned yMin, unsigned yMax);
    Vector3d bounce(Scene& scene, Light* light, Ray& ray,
                    std::default_random_engine& r, unsigned nBounces = 0) const;
};


template<typename T_Camera>
void Renderer::render(Camera<T_Camera>& camera, Scene& scene, Light* light, Canvas& canvas,
                std::default_random_engine& r) {
    for (auto& thread : threads_)
        thread.join();
    threads_.clear();

    printf("Launching threads..\n");
    for (auto i=0u; i<maxThreads_; ++i) {

        unsigned yMin = ceil(((float)canvas.getHeight() / maxThreads_)*i);
        unsigned yMax = ceil(((float)canvas.getHeight() / maxThreads_)*(i+1));

        threads_.emplace_back([yMin, yMax, &camera, &scene, light, &canvas, &r, this]
                              { renderAsync(camera, scene, light, canvas, r,
                                            0, canvas.getWidth(), yMin, yMax); } );
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
void Renderer::renderAsync(Camera<T_Camera>& camera, Scene& scene, Light* light, Canvas& canvas,
                           std::default_random_engine& r,
                           unsigned xMin, unsigned xMax, unsigned yMin, unsigned yMax) {
    unsigned viewW = canvas.getWidth();
    unsigned viewH = canvas.getHeight();

    Ray ray;

    for (auto y=yMin; y<yMax; ++y) {
        for (auto x=xMin; x<xMax; ++x) {
            //printf("%u, %u: ", x, y);
            for (auto i=0u; i<2; ++i) {
                for (auto j=0u; j<2; ++j) {
                    float rayX = x+((r() % 65536) / 65534.0f);
                    float rayY = y+((r() % 65536) / 65534.0f);
                    ray = camera.generateRay(rayX, rayY, viewW, viewH);
                    Vector3d pathLight = bounce(scene, light, ray, r, 1);

                    canvas.addSample({rayX, rayY},
                                     {pathLight[0], pathLight[1], pathLight[2]});
                }
            }
        }
        //printf("%0.3f%%\r", (float)((y-yMin)*100)/(yMax-yMin));
    }
    //printf("\n");
}


#endif  //  RENDERER_HPP
