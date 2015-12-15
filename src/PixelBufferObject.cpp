#include "PixelBufferObject.hpp"


PixelBufferObject::PixelBufferObject(void) :
    pixelBufferId_(0)
{
    glGenBuffers(1, &pixelBufferId_);
}

PixelBufferObject::~PixelBufferObject(void) {
    if (pixelBufferId_)
        glDeleteBuffers(1, &pixelBufferId_);
}

GLubyte* PixelBufferObject::map(unsigned long size) const {
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixelBufferId_);

    glBufferData(GL_PIXEL_UNPACK_BUFFER, size, 0, GL_STREAM_DRAW);

    // map the buffer object into client's memory
    return (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
}

void PixelBufferObject::unmapAndUpdate(const Texture& texture) const {
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB); // release the mapped buffer

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, texture.width(), texture.height(), GL_RGBA, GL_UNSIGNED_BYTE, (void*)0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
