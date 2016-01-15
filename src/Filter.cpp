#include "Filter.hpp"
#include "MathTypes.hpp"

#include <cmath>


Filter::Filter(Type type, float width) :
    type_(type), width_(width)
{}

void Filter::setWidth(float width) {
    width_ = width;
}

float Filter::getWidth(void) const {
    return width_;
}

float Filter::operator()(float sampleX, float sampleY,
                         float x0, float x1, float y0, float y1) const {
    return (*this)(sampleX, sampleY, x0, x1, y0, y1, width_);
}

float Filter::operator()(float sampleX, float sampleY,
                         float x0, float x1, float y0, float y1, float width) const {
    float widthInv = 1.0f/width;
    x0 -= sampleX;
    x1 -= sampleX;
    y0 -= sampleY;
    y1 -= sampleY;

    switch (type_) {
    case TYPE_DIRACDELTA:
        //  TODO_IMPLEMENT
    break;
    case TYPE_TENT:
        return (3.0f/(PI*width*width))*(width-sqrt(((x0+x1)*(x0+x1))/4.0f + ((y0+y1)*(y0+y1))/4.0f));
    break;
    case TYPE_GAUSSIAN: //  TODO_FIX
        return 0.25f*(std::erf(0.5*x0*widthInv)-std::erf(0.5*x1*widthInv))
                    *(std::erf(0.5*y0*widthInv)-std::erf(0.5*y1*widthInv));
    break;
    }

    return 1.0f;
}
