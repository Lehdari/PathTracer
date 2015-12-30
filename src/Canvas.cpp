#include "Canvas.hpp"

#include <SFML/Graphics/Image.hpp>


Canvas::Canvas(Filter& filter, unsigned width, unsigned height) :
    filter_(filter), width_(width), height_(height),
    pixData_(height_, std::vector<Vector3d>(width_, Vector3d{0.0, 0.0, 0.0})),
    pixDataMax_(0.0)
{
    texture_.clear(width_, height_);
    texture_.setAttribute(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    texture_.setAttribute(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Canvas::setFilter(Filter& filter) {
    filter_ = filter;
}

const Filter& Canvas::getFilter(void) const {
    return filter_;
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

    int xMin = pos[0]-filter_.width_;
    if (xMin < 0) xMin = 0;
    int yMin = pos[1]-filter_.width_;
    if (yMin < 0) yMin = 0;
    int xMax= pos[0]+filter_.width_+1.0f;
    if (xMax > (int)width_) xMax = width_;
    int yMax= pos[1]+filter_.width_+1.0f;
    if (yMax > (int)height_) yMax = height_;

    for (auto y=yMin; y<yMax; ++y) {
        for (auto x=xMin; x<xMax; ++x) {
            if (sqrtf(powf(x+0.5f-pos[0],2.0f)+powf(y+0.5f-pos[1],2.0f)) > filter_.width_)
                continue;

            pixData_[y][x]+=filter_(pos[0], pos[1], x, x+1, y, y+1)*val;
            for (auto i=0u; i<3; ++i)
                if (pixData_[y][x][i] > pixDataMax_)
                    pixDataMax_ = pixData_[y][x][i];
        }
    }

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
                         sf::Color((p[0] / pixDataMax_)*255,
                                   (p[1] / pixDataMax_)*255,
                                   (p[2] / pixDataMax_)*255));
        }
    }

    img.saveToFile(fileName);
}
