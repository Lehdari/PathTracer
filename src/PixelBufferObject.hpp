#ifndef PIXELBUFFEROBJECT_HPP
#define PIXELBUFFEROBJECT_HPP


#include "Texture.hpp"

#include <GL/glew.h>


class PixelBufferObject {
public:
    PixelBufferObject(void);
    ~PixelBufferObject(void);

    PixelBufferObject(const PixelBufferObject& other)             = delete;
    PixelBufferObject(PixelBufferObject&& other)                  = delete;
    PixelBufferObject& operator=(const PixelBufferObject& other)  = delete;
    PixelBufferObject& operator=(PixelBufferObject&& other)       = delete;

    GLubyte* map(unsigned long size) const;
    void unmapAndUpdate(Texture& texture) const;

private:
    GLuint pixelBufferId_;
};


#endif  //  PIXELBUFFEROBJECT_HPP
