#include "Sampler.hpp"

#include <cmath>


Sampler::Sampler(Sampler::Type type, unsigned size) :
    type_(type), size_(size), sizeSqrt_(sqrt(size_))
{}


void Sampler::drawSample(float& x, float& y, unsigned id) {
    switch (type_) {
    case TYPE_REGULAR:
        x = id % sizeSqrt_;
        y = id / sizeSqrt_;
    break;
    case TYPE_UNIFORM:
    break;
    case TYPE_JITTERED:
    break;
    }
}
