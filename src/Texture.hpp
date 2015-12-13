#ifndef TEXTURE_HPP
#define TEXTURE_HPP


#include <GL/glew.h>
#include <SFML/Graphics/Image.hpp>


class Texture {
public:
    Texture(void);
    ~Texture(void);

    Texture(const Texture& other)             = delete;
    Texture(Texture&& other)                  = delete;
    Texture& operator=(const Texture& other)  = delete;
    Texture& operator=(Texture&& other)       = delete;

    operator GLuint(void) const;

    GLuint getID(void) const;
    unsigned width(void) const;
    unsigned height(void) const;

    void clear(unsigned width, unsigned height,
               GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);
    void loadFromImage(const sf::Image& image);

private:
    GLuint textureId_;
    unsigned width_;
    unsigned height_;
};


#endif // TEXTURE_HPP
