#include "Sample.hpp"


Sample::Sample(const Vector2f& pos, const Vector3d& col) :
    pos_    (pos),
    col_    (col)
{
}

void Sample::getPos(float& x, float& y) const
{
    x = pos_[0];
    y = pos_[1];
}

void Sample::getCol(Vector3d& col) const
{
    col = col_;
}
