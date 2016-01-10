#ifndef CANVAS_HPP
#define CANVAS_HPP


#include "Filter.hpp"
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

struct PixData {
    Vector3d c; //  accumulated color data
    double w;   //  weight
};

class Canvas {
public:
    Canvas(Filter& filter, unsigned width, unsigned height);

    void setFilter(Filter& filter);

    const Filter& getFilter(void) const;
    unsigned getWidth(void) const;
    unsigned getHeight(void) const;

    const Texture& getTexture(void);

    void addSample(const Vector2f& pos, const Vector3d& val);
    void clear(void);
    void filter(/*Filter& filter, */float gamma);
    void saveToFile(const std::string& fileName);
    void normalize(void);

private:
    Filter& filter_;
    unsigned width_;
    unsigned height_;

    //  pixel data
    std::vector<std::vector<PixData>> pixData_;
    double normConst_;  //  normalization constant
    int pdmx_, pdmy_;
    bool pixDataDirty_;
    std::mutex pixDataMutex_;

    Texture texture_;
    PixelBufferObject pixelBuffer_;
};


#endif  //  CANVAS_HPP
