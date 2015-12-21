#include "Canvas.hpp"

#include <SFML/Graphics/Image.hpp>


Canvas::Canvas(unsigned width, unsigned height) :
    width_(width), height_(height),
    pixData_(height_, std::vector<Vector3d>(width_, Vector3d{0.0, 0.0, 0.0})),
    pixDataMax_(0.0)
{
    texture_.clear(width_, height_);
    texture_.setAttribute(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    texture_.setAttribute(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

unsigned Canvas::getWidth(void) const {
    return width_;
}

unsigned Canvas::getHeight(void) const {
    return height_;
}

const Texture& Canvas::getTexture(void) {
    std::lock_guard<std::mutex> lock(pixDataMutex_);

    if (pixDataDirty_) {
        auto ptr = pixelBuffer_.map(texture_.width()*texture_.height()*4*sizeof(unsigned char));
        if(ptr) {
            for (auto y=0u; y<height_; ++y) {
                for (auto x=0u; x<width_; ++x) {
                    ptr[(y*width_ + x)*4 + 0] = (pixData_[y][x][0] / pixDataMax_)*255;
                    ptr[(y*width_ + x)*4 + 1] = (pixData_[y][x][1] / pixDataMax_)*255;
                    ptr[(y*width_ + x)*4 + 2] = (pixData_[y][x][2] / pixDataMax_)*255;
                    ptr[(y*400 + x)*4 + 3] = 255;
                }
            }
            pixelBuffer_.unmapAndUpdate(texture_);
        }

        pixDataDirty_ = false;
    }

    return texture_;
}

void Canvas::addSample(const Vector2f& pos, const Vector3d& val) {
    std::lock_guard<std::mutex> lock(pixDataMutex_);

    pixData_[(unsigned)pos[1]][(unsigned)pos[0]] += val;
    for (auto i=0u; i<3; ++i)
        if (pixData_[(unsigned)pos[1]][(unsigned)pos[0]][i] > pixDataMax_)
            pixDataMax_ = pixData_[(unsigned)pos[1]][(unsigned)pos[0]][i];

    pixDataDirty_ = true;
}

void Canvas::clear(void) {
    //samples_.clear();
    pixData_ = std::vector<std::vector<Vector3d>>(height_, std::vector<Vector3d>(width_, Vector3d{0.0, 0.0, 0.0}));
}

void Canvas::filter(/*Filter& filter, */float gamma) {
    /*for (auto& s : samples_) {
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
                p[i] = pow((p[i]-min)*n, gamma);*/
}

void Canvas::saveToFile(const std::string& fileName) {
    std::lock_guard<std::mutex> lock(pixDataMutex_);

    sf::Image img;
    img.create(width_, height_);

    for (auto y=0u; y<height_; ++y) {
        for (auto x=0u; x<width_; ++x) {
            auto& p = pixData_[y][x];
            img.setPixel(x, height_ - y - 1,
                         sf::Color((pixData_[y][x][0] / pixDataMax_)*255,
                                   (pixData_[y][x][1] / pixDataMax_)*255,
                                   (pixData_[y][x][2] / pixDataMax_)*255));
        }
    }

    img.saveToFile(fileName);
}
