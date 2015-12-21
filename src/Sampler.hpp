#ifndef SAMPLER_HPP
#define SAMPLER_HPP


#include <random>


class Sampler {
public:
    enum Type {
        TYPE_REGULAR,
        TYPE_UNIFORM,
        TYPE_JITTERED
    };

    Sampler(Type type, unsigned size);
    void drawSample(float& x, float& y, unsigned id,
                    std::default_random_engine& r);

private:
    Type type_;
    unsigned size_;
    unsigned sizeSqrt_;
};


#endif  //  SAMPLER_HPP
