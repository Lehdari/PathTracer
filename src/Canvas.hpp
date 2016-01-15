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

    //  auxiliary data
    Vector3f dx, dy;   //  derivatives
    float e;    //  estimated error value
};

class Canvas {
public:
    Canvas(Filter& filter, unsigned width, unsigned height);

    void setFilter(Filter& filter);

    const Filter& getFilter(void) const;
    unsigned getWidth(void) const;
    unsigned getHeight(void) const;

    const Texture& getTexture(void);

    void addSample(const Vector2f& pos, const Vector3d& val, float filterWidth);
    void clear(void);
    void saveToFile(const std::string& fileName);
    void normalize(void);
    void generateAuxiliarMaps(void);

    float getSamplingProbability(unsigned x, unsigned y);

private:
    Filter& filter_;
    unsigned width_;
    unsigned height_;

    //  pixel data
    std::vector<std::vector<PixData>> pixData_;
    double normConst_;  //  normalization constant
    double maxWeight_;
    bool pixDataDirty_;
    std::mutex pixDataMutex_;

    Texture texture_;
    PixelBufferObject pixelBuffer_;
};


#endif  //  CANVAS_HPP
