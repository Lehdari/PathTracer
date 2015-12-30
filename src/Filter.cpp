#include "Filter.hpp"
#include <cmath>


Filter::Filter(Type type, float width) :
    type_(type), width_(width), widthInv_(1.0f/width_)
{}

void Filter::setWidth(float width) {
    width_ = width;
    widthInv_ = 1.0f/width_;
}

float Filter::getWidth(void) const {
    return width_;
}

float Filter::operator()(float sampleX, float sampleY,
                         float x0, float x1, float y0, float y1) const {
    x0 -= sampleX;
    x1 -= sampleX;
    y0 -= sampleY;
    y1 -= sampleY;

    switch (type_) {
    case TYPE_DIRACDELTA:
        //  TODO_IMPLEMENT
    break;
    case TYPE_TENT:
        //  TODO_IMPLEMENT
    break;
    case TYPE_GAUSSIAN:
        return 0.25f*(std::erf(0.5*x0*widthInv_)-std::erf(0.5*x1*widthInv_))
                    *(std::erf(0.5*y0*widthInv_)-std::erf(0.5*y1*widthInv_));
    break;
    }

    return 1.0f;
}
