#include "Sampler.hpp"

#include <cmath>


Sampler::Sampler(Sampler::Type type, unsigned size) :
    type_(type), size_(size), sizeSqrt_(sqrt(size_))
{}


void Sampler::drawSample(float& x, float& y, unsigned id,
                         std::default_random_engine& r) {
    switch (type_) {
    case TYPE_REGULAR:
        x = ((id % sizeSqrt_) + 0.5f) / sizeSqrt_;
        y = ((id / sizeSqrt_) + 0.5f) / sizeSqrt_;
    break;
    case TYPE_UNIFORM:
        x = (r()%1024)/1024.0f;
        y = (r()%1024)/1024.0f;
    break;
    case TYPE_JITTERED:
        x = ((id % sizeSqrt_) + (r()%1024)/1024.0f) / sizeSqrt_;
        y = ((id / sizeSqrt_) + (r()%1024)/1024.0f) / sizeSqrt_;
    break;
    }
}
