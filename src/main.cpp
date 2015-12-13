#include "Scene.hpp"
#include "BasicCamera.hpp"
#include "PointLight.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "PixelBufferObject.hpp"

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
    window.setFramerateLimit(60);
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
    //PointLight light({0.55f, -0.25f, -0.6f}, {1.2f, 1.15f, 1.0f});
    PointLight light({0.0f, 3.25f, -0.0f}, {1.2f, 1.15f, 1.0f});
    //scene.addLight(&light);

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

    //  Canvas
    Canvas canvas(400, 300);

    //  Mesh
    Mesh mesh;
    mesh.loadFromObj("res/scenes/cornell.obj");

    //  Shader
    Shader shader("res/shaders/VS_Simple.glsl", "res/shaders/FS_Simple.glsl");

    std::default_random_engine r(time(NULL));

    float t = 0.0f;


    //  Pixel buffer object
    sf::Image img;
    //img.create(400, 300, sf::Color(255, 200, 100));
    img.loadFromFile("render.png");

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

    Texture texture;
    texture.loadFromImage(img);
/*
    GLuint pixelBuffer;
    glGenBuffers(1, &pixelBuffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixelBuffer);

    glBufferData(GL_PIXEL_UNPACK_BUFFER, 400*300*4*sizeof(unsigned char), 0, GL_STREAM_DRAW);
*/
    PixelBufferObject pixelBuffer;

    // map the buffer object into client's memory
    GLubyte* ptr = pixelBuffer.map(texture.width()*texture.height()*4*sizeof(unsigned char));
    if(ptr) {
        // update data directly on the mapped buffer
        for (auto y=0u; y<300; ++y) {
            for (auto x=0u; x<400; ++x) {
                ptr[(y*400 + x)*4 + 0] = x/2;
                ptr[(y*400 + x)*4 + 1] = y/2;
                ptr[(y*400 + x)*4 + 2] = 255;
                ptr[(y*400 + x)*4 + 3] = 255;
            }
        }
        pixelBuffer.unmapAndUpdate(texture);
    }

    //glBindTexture (GL_TEXTURE_2D, texture);
    //glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, 400, 300, GL_RGBA, GL_UNSIGNED_BYTE, (void*)0);

    // it is good idea to release PBOs with ID 0 after use.
    // Once bound with 0, all pixel operations are back to normal ways.
    //glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

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
                switch(event.key.code) {
                case sf::Keyboard::Escape:
                    window.close();
                break;
                case sf::Keyboard::Space:
                    //  Render
                    for (auto i=0u; i<16; ++i)
                        camera.render(scene, &light, canvas, r);
                    canvas.filter(0.75);
                    canvas.saveToFile("render.png");
                    canvas.clear();
                break;
                default:
                break;
                }
            default:
            break;
            }
        }



        //glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixelBuffer);
        //glBufferData(GL_PIXEL_UNPACK_BUFFER, 400*300*4*sizeof(unsigned char), 0, GL_STREAM_DRAW);

        // map the buffer object into client's memory
        ptr = pixelBuffer.map(texture.width()*texture.height()*4*sizeof(unsigned char));
        if(ptr) {
            // update data directly on the mapped buffer
            for (auto y=0u; y<300; ++y) {
                for (auto x=0u; x<400; ++x) {
                    ptr[(y*400 + x)*4 + 0] = (0.5+0.5*sin(2*PI*0.25*t))*(0.5+0.5*sin(x*0.1*sin(2*PI*0.13*t)))*255;
                    ptr[(y*400 + x)*4 + 1] = (0.5+0.5*sin(2*PI*0.2*t))*(0.5+0.5*sin(y*0.05*sin(2*PI*0.11*t)))*255;
                    ptr[(y*400 + x)*4 + 2] = (0.5+0.5*sin(2*PI*0.15*t))*255;
                    ptr[(y*400 + x)*4 + 3] = 255;
                }
            }
            pixelBuffer.unmapAndUpdate(texture);
        }

        //glBindTexture (GL_TEXTURE_2D, texture);
        //glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, 400, 300, GL_RGBA, GL_UNSIGNED_BYTE, (void*)0);

        // it is good idea to release PBOs with ID 0 after use.
        // Once bound with 0, all pixel operations are back to normal ways.
        //glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);



        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(imgShader.getId());

        glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
        glBindTexture(GL_TEXTURE_2D, texture);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindBuffer(GL_VERTEX_ARRAY, 0);

        //mesh.render(shader, camera);
        /*camera.lookAt( {0.8f*cosf(t*PI*2*0.125f), 0.5f+0.1f*cosf(t*PI*2*0.021f), 0.8f*sinf(t*PI*2*0.125f)},
                       {0.0f, 0.0f, 0.0f},
                       {0.0f, 1.0f, 0.0f} );*/

        t += 1.0f/60.0f;

        window.display();
    }

    glDeleteBuffers(1, &quadVertexBuffer);

    return 0;
}
