#include "Scene.hpp"
#include "BasicCamera.hpp"
#include "PointLight.hpp"
#include "Renderer.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "PixelBufferObject.hpp"
#include "Keyboard.hpp"
#include "EventFunctions.hpp"

#include <SFML/Graphics.hpp>
#include <ctime>
#include <iostream>


int main(void) {
    // Declare and create a new window
    sf::ContextSettings settings;
    settings.depthBits         = 24; // Request a 24 bits depth buffer
    settings.stencilBits       = 8;  // Request a 8 bits stencil buffer
    settings.antialiasingLevel = 8;  // Request 2 levels of antialiasing
    settings.majorVersion = 3;
    settings.minorVersion = 0;
    sf::Window window(sf::VideoMode(800, 600, 32), "RayTracer", sf::Style::Default, settings);
    // Activate the window for OpenGL rendering
    window.setActive();
    // Limit the framerate to 60 frames per second (this step is optional)
    window.setFramerateLimit(30);
    //  Initialize GLEW
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    //  Scene
    Scene scene;
    scene.loadFromObj("res/scenes/cornell.obj");

    //  Light
    //  cornell
    PointLight light({0.55f, -0.25f, -0.6f}, {1.2f, 1.15f, 1.0f});
    //  hub
    //PointLight light({0.0f, 3.25f, -0.0f}, {1.2f, 1.15f, 1.0f});

    //  Camera
    BasicCamera camera(PI*0.5f, 4.0f/3.0f, 0.2f, 100.0f);
    //  cornell 1
    /*camera.lookAt( {0.7f, -0.8f, -0.95f},
                   {0.0f, -0.5f, 0.0f},
                   {0.0f, 1.0f, 0.0f} );*/
    //  cornell 2
    camera.lookAt( {-0.9f, 0.2f, 0.4f},
                   {0.0f, -0.6f, 0.1f},
                   {0.0f, 1.0f, 0.0f} );
    //  hub
    /*camera.lookAt( {-5.9f, 1.2f, -1.4f},
                   {0.0f, 2.2f, 0.1f},
                   {0.0f, 1.0f, 0.0f} );*/

    //  Filter
    Filter filter(Filter::TYPE_GAUSSIAN, 3.0f);

    //  Canvas
    Canvas canvas(filter, 400, 300);

    //  Renderer
    Renderer renderer(8);

    //  Mesh
    Mesh mesh;
    mesh.loadFromObj("res/scenes/cornell.obj");

    //  Shader
    Shader shader("res/shaders/VS_Simple.glsl", "res/shaders/FS_Simple.glsl");

    //  Render result display stuff
    Shader imgShader("res/shaders/VS_Image.glsl", "res/shaders/FS_Image.glsl");

    static const float quad[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f,  -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f,  -1.0f, 0.0f,
        1.0f,   1.0f, 0.0f
    };

    GLuint quadVertexBuffer;
    glGenBuffers(1, &quadVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad[0], GL_STATIC_DRAW);

    //  Some other stuff
    std::default_random_engine r(time(NULL));
    float t = 0.0f;
    bool showRender = false;

    //  Keyboard and events setup
    Keyboard keyboard;
    keyboard.setKeyDownFunction(sf::Keyboard::W,
        std::bind(&moveCamera, std::ref(camera), true, Vector3f{0.0f, 0.0f, -0.025f}));
    keyboard.setKeyDownFunction(sf::Keyboard::S,
        std::bind(&moveCamera, std::ref(camera), true, Vector3f{0.0f, 0.0f, 0.025f}));
    keyboard.setKeyDownFunction(sf::Keyboard::D,
        std::bind(&moveCamera, std::ref(camera), true, Vector3f{0.025f, 0.0f, 0.0f}));
    keyboard.setKeyDownFunction(sf::Keyboard::A,
        std::bind(&moveCamera, std::ref(camera), true, Vector3f{-0.025f, 0.0f, 0.0f}));
    keyboard.setKeyDownFunction(sf::Keyboard::E,
        std::bind(&moveCamera, std::ref(camera), true, Vector3f{0.0f, 0.025f, 0.0f}));
    keyboard.setKeyDownFunction(sf::Keyboard::Q,
        std::bind(&moveCamera, std::ref(camera), true, Vector3f{0.0f, -0.025f, 0.0f}));

    keyboard.setKeyPressedFunction(sf::Keyboard::Space,
        std::bind(&render, std::ref(renderer), std::ref(camera), std::ref(scene),
                  &light, std::ref(canvas),
                  std::ref(r), std::ref(showRender), std::ref(filter)));
    keyboard.setKeyPressedFunction(sf::Keyboard::Return,
        std::bind(&toggleShowRender, std::ref(showRender)));


    // The main loop - ends as soon as the window is closed
    while (window.isOpen())
    {
        // Event processing
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
            break;
            case sf::Event::KeyPressed:
                keyboard.keyPressed(event.key.code);
                /*switch(event.key.code) {
                case sf::Keyboard::Escape:
                    window.close();
                break;
                case sf::Keyboard::Space: {
                    //  Render
                    renderer.render(camera, scene, &light, canvas, r);

                    if (!showRender)
                        showRender = true;
                } break;
                case sf::Keyboard::Return:
                    showRender = !showRender;
                break;
                case sf::Keyboard::P:
                    canvas.saveToFile("render.png");
                break;
                case sf::Keyboard::W:
                    camera.moveLocal({0.0f, 0.0f, 0.05f});
                break;
                default:
                break;
                }*/
            break;
            case sf::Event::KeyReleased:
                keyboard.keyReleased(event.key.code);
            break;
            default:
            break;
            }
        }
        keyboard.callKeyDownFunctions();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!showRender) {
            mesh.render(shader, camera);
            /*camera.lookAt( {0.8f*cosf(t*PI*2*0.125f), 0.5f+0.1f*cosf(t*PI*2*0.021f), 0.8f*sinf(t*PI*2*0.125f)},
                           {0.0f, 0.0f, 0.0f},
                           {0.0f, 1.0f, 0.0f} );*/
        }
        else {
            glUseProgram(imgShader.getId());

            glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
            glBindTexture(GL_TEXTURE_2D, canvas.getTexture());

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindBuffer(GL_VERTEX_ARRAY, 0);
        }

        t += 1.0f/60.0f;

        window.display();
    }

    //  Cleanup
    glDeleteBuffers(1, &quadVertexBuffer);

    return 0;
}
