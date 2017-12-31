#include "Sample.hpp"


Sample::Sample(const Vector2f& pos, const Vector3d& col, double prob) :
    pos_    (pos),
    col_    (col),
    prob_   (prob)
{
}

void Sample::getPos(float& x, float& y) const
{
    x = pos_[0];
    y = pos_[1];
}

void Sample::getCol(Vector3d& col) const
{
    col = col_ / prob_;
}
