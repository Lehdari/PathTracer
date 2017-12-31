#ifndef CANVAS_HPP
#define CANVAS_HPP


#include "Filter.hpp"
#include "MathTypes.hpp"
#include "Texture.hpp"
#include "PixelBufferObject.hpp"
#include "Sample.hpp"

#include <string>
#include <vector>
#include <mutex>


class Canvas {
public:
    Canvas(Filter& filter, unsigned width, unsigned height);

    void setFilter(Filter& filter);

    const Filter& getFilter(void) const;
    unsigned getWidth(void) const;
    unsigned getHeight(void) const;

    const Texture& getTexture(void);

    void addSample(const Sample& sample);
    void clear(void);
    void filter(/*Filter& filter, */float gamma);
    void saveToFile(const std::string& fileName);

private:
    Filter& filter_;
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
