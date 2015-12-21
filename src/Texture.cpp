#include "Texture.hpp"


Texture::Texture(void) :
    textureId_(0),
    width_(0),
    height_(0)
{
    glGenTextures(1, &textureId_);
    glBindTexture(GL_TEXTURE_2D, textureId_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture(void) {
    if (textureId_)
        glDeleteTextures(1, &textureId_);
}

Texture::operator GLuint(void) const {
    return textureId_;
}

GLuint Texture::getID(void) const {
    return textureId_;
}

unsigned Texture::width(void) const {
    return width_;
}

unsigned Texture::height(void) const {
    return height_;
}

void Texture::clear(unsigned width, unsigned height, GLenum format, GLenum type) {
    width_ = width;
    height_ = height;

    glBindTexture(GL_TEXTURE_2D, textureId_);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0, format, type, (void*)0);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::loadFromImage(const sf::Image& image) {
    auto s = image.getSize();
    width_ = s.x;
    height_ = s.y;

    glBindTexture(GL_TEXTURE_2D, textureId_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setAttribute(GLenum attr, GLenum val) {
    glBindTexture(GL_TEXTURE_2D, textureId_);
    glTexParameteri(GL_TEXTURE_2D, attr, val);
    glBindTexture(GL_TEXTURE_2D, 0);
}
