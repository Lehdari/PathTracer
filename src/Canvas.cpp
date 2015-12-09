#include "Canvas.hpp"

#include <SFML/Graphics/Image.hpp>


Canvas::Canvas(unsigned width, unsigned height) :
    width_(width), height_(height),
    pixData_(height_, std::vector<Vector3d>(width_, Vector3d{0.0, 0.0, 0.0}))
{}

unsigned Canvas::getWidth(void) const {
    return width_;
}

unsigned Canvas::getHeight(void) const {
    return height_;
}

void Canvas::addSample(const Vector2f& pos, const Vector3d& val) {
    if (pos[0] > 0.0f && pos[0] < width_ && pos[1] > 0.0f && pos[1] < height_)
        samples_.push_back({pos, val});
}

void Canvas::clear(void) {
    samples_.clear();
    pixData_ = std::vector<std::vector<Vector3d>>(height_, std::vector<Vector3d>(width_, Vector3d{0.0, 0.0, 0.0}));
}

void Canvas::filter(/*Filter& filter, */float gamma) {
    for (auto& s : samples_) {
        pixData_[(unsigned)s.p[1]][(unsigned)s.p[0]] += s.v;
    }

    //  normalization and gamma correction
    double min(pixData_[0][0][0]), max(pixData_[0][0][0]);

    for (auto& row : pixData_) {
        for (auto& p : row) {
            for (auto i=0u; i<3; ++i) {
                if (p[i] < min)
                    min = p[i];
                if (p[i] > max)
                    max = p[i];
            }
        }
    }

    double n = 1.0/(max-min);

    for (auto& row : pixData_)
        for (auto& p : row)
            for (auto i=0u; i<3; ++i)
                p[i] = pow((p[i]-min)*n, gamma);
}

void Canvas::saveToFile(const std::string& fileName) const {
    sf::Image img;
    img.create(width_, height_);

    for (auto y=0u; y<height_; ++y) {
        for (auto x=0u; x<width_; ++x) {
            auto& p = pixData_[y][x];
            img.setPixel(x, y, sf::Color(p[0]*255, p[1]*255, p[2]*255));
        }
    }

    img.saveToFile(fileName);
}
