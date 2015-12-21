#ifndef SAMPLER_HPP
#define SAMPLER_HPP


class Sampler {
public:
    enum Type {
        TYPE_REGULAR,
        TYPE_UNIFORM,
        TYPE_JITTERED
    };

    Sampler(Type type, unsigned size);
    void drawSample(float& x, float& y, unsigned id);

private:
    Type type_;
    unsigned size_;
    unsigned sizeSqrt_;
};


#endif  //  SAMPLER_HPP
