#ifndef CANVAS_HPP
#define CANVAS_HPP


#include "MathTypes.hpp"
#include "Texture.hpp"
#include "PixelBufferObject.hpp"

#include <string>
#include <vector>
#include <mutex>


struct Sample {
    Vector2f p; //  position on canvas
    Vector3d v; //  value
};

class Canvas {
public:
    Canvas(unsigned width, unsigned height);

    unsigned getWidth(void) const;
    unsigned getHeight(void) const;

    const Texture& getTexture(void);

    void addSample(const Vector2f& pos, const Vector3d& val);
    void clear(void);
    void filter(/*Filter& filter, */float gamma);
    void saveToFile(const std::string& fileName);

private:
    unsigned width_;
    unsigned height_;

    //  double-precision HDR pixel data
    std::vector<std::vector<Vector3d>> pixData_;
    double pixDataMax_;
    bool pixDataDirty_;
    std::mutex pixDataMutex_;

    Texture texture_;
    PixelBufferObject pixelBuffer_;

};


#endif  //  CANVAS_HPP
