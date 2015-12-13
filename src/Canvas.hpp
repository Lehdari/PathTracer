#ifndef CANVAS_HPP
#define CANVAS_HPP


#include "MathTypes.hpp"
#include "Texture.hpp"
#include "PixelBufferObject.hpp"

#include <string>
#include <vector>


struct Sample {
    Vector2f p; //  position on canvas
    Vector3d v; //  value
};

class Canvas {
public:
    Canvas(unsigned width, unsigned height);

    unsigned getWidth(void) const;
    unsigned getHeight(void) const;

    void addSample(const Vector2f& pos, const Vector3d& val);
    void clear(void);
    void filter(/*Filter& filter, */float gamma);
    void saveToFile(const std::string& fileName) const;

private:
    unsigned width_;
    unsigned height_;
    std::vector<Sample> samples_;

    //  double-precision HDR pixel data, created by filter method
    std::vector<std::vector<Vector3d>> pixData_;
};


#endif  //  CANVAS_HPP
