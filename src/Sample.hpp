#ifndef SAMPLE_HPP
#define SAMPLE_HPP


#include "MathTypes.hpp"


class Sample {
public:
    Sample(const Vector2f& pos, const Vector3d& col);

    void getPos(float& x, float& y) const;
    void getCol(Vector3d& col) const;

private:
    const Vector2f  pos_;
    Vector3d        col_;
};


#endif // SAMPLE_HPP
