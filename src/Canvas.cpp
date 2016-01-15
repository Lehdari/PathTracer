#include "Canvas.hpp"

#include <SFML/Graphics/Image.hpp>


Canvas::Canvas(Filter& filter, unsigned width, unsigned height) :
    filter_(filter), width_(width), height_(height),
    pixData_(height_, std::vector<PixData>(width_, {
                                                      {0.0, 0.0, 0.0},
                                                      0.0,
                                                      {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
                                                      0.1f
                                                   })),
    normConst_(1.0), maxWeight_(0.0)
{
    texture_.clear(width_*2, height_*2);
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
                    auto& p = pixData_[y][x];
                    unsigned pix = (y*width_*2 + x)*4;
                    if (p.w > 0.0) {
                        ptr[pix + 0] = (p.c[0] / (normConst_*p.w))*255;
                        ptr[pix + 1] = (p.c[1] / (normConst_*p.w))*255;
                        ptr[pix + 2] = (p.c[2] / (normConst_*p.w))*255;
                        //if (x == pdmx_ && y == pdmy_)p.dx[0]+
                            //printf("Pix data max info:\n  pos: [%u, %u]\n  rgb: [%u, %u, %u]\n  c: [%0.2f, %0.2f, %0.2f]\n  w: %0.3f  pdm: %0.3f\n", x, y, ptr[(y*width_ + x)*4 + 0], ptr[(y*width_ + x)*4 + 1], ptr[(y*width_ + x)*4 + 2], p.c[0], p.c[1], p.c[2], p.w, normConst_);
                    }
                    else {
                        ptr[pix + 0] = 0;
                        ptr[pix + 1] = 0;
                        ptr[pix + 2] = 0;
                    }
                    ptr[pix + 3] = 255;

                    //  auxiliary maps - TEMP
                    unsigned pix2 = (y*width_*2 + x + width_)*4;
                    ptr[pix2 + 0] = p.e*255;
                    ptr[pix2 + 1] = p.e*255;
                    ptr[pix2 + 2] = p.e*255;
                    ptr[pix2 + 3] = 255;

                    unsigned pix3 = ((y+height_)*width_*2 + x)*4;
                    ptr[pix3 + 0] = (p.w/maxWeight_)*255;
                    ptr[pix3 + 1] = (p.w/maxWeight_)*255;
                    ptr[pix3 + 2] = (p.w/maxWeight_)*255;
                    ptr[pix3 + 3] = 255;

                    //unsigned pix4 = ((y+height_)*width_*2 + x + width_)*4;
                    //ptr[pix4 + 0] = p.e*255;
                    //ptr[pix4 + 1] = p.e*255;
                    //ptr[pix4 + 2] = p.e*255;
                    //ptr[pix4 + 3] = 255;
                }
            }
            pixelBuffer_.unmapAndUpdate(texture_);
        }

        pixDataDirty_ = false;
    }

    return texture_;
}

void Canvas::addSample(const Vector2f& pos, const Vector3d& val, float filterWidth) {
    std::lock_guard<std::mutex> lock(pixDataMutex_);

    int xMin = pos[0]-filterWidth;
    if (xMin < 0) xMin = 0;
    int yMin = pos[1]-filterWidth;
    if (yMin < 0) yMin = 0;
    int xMax= pos[0]+filterWidth+1.0f;
    if (xMax > (int)width_) xMax = width_;
    int yMax= pos[1]+filterWidth+1.0f;
    if (yMax > (int)height_) yMax = height_;

    for (auto y=yMin; y<yMax; ++y) {
        for (auto x=xMin; x<xMax; ++x) {
            if (sqrtf(powf(x+0.5f-pos[0],2.0f)+powf(y+0.5f-pos[1],2.0f)) > filterWidth)
                continue;

            float f = filter_(pos[0], pos[1], x, x+1, y, y+1, filterWidth);

            pixData_[y][x].c += f*val;
            pixData_[y][x].w += f;
            if (pixData_[y][x].w > maxWeight_)
                maxWeight_ = pixData_[y][x].w;
        }
    }

    pixDataDirty_ = true;
}

void Canvas::clear(void) {
    //samples_.clear();
    pixData_ = std::vector<std::vector<PixData>>(height_, std::vector<PixData>(width_,
               {
                   {0.0, 0.0, 0.0},
                   0.0,
                   {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
                   1.0f
               }));
}

void Canvas::saveToFile(const std::string& fileName) {
    std::lock_guard<std::mutex> lock(pixDataMutex_);

    sf::Image img;
    img.create(width_, height_);

    for (auto y=0u; y<height_; ++y) {
        for (auto x=0u; x<width_; ++x) {
            auto& p = pixData_[y][x];
            if (p.w > 0.0) {
                img.setPixel(x, height_ - y - 1,
                             sf::Color((p.c[0] / (normConst_*p.w))*255,
                                       (p.c[1] / (normConst_*p.w))*255,
                                       (p.c[2] / (normConst_*p.w))*255));
            }
        }
    }

    img.saveToFile(fileName);
}

void Canvas::normalize(void) {
    std::lock_guard<std::mutex> lock(pixDataMutex_);

    normConst_ = 0.0;
    for (auto y=0u; y<height_; ++y)
        for (auto x=0u; x<width_; ++x)
            for (auto i=0u; i<3; ++i)
                if (pixData_[y][x].c[i]/pixData_[y][x].w > normConst_)
                    normConst_ = pixData_[y][x].c[i]/pixData_[y][x].w;
}

void Canvas::generateAuxiliarMaps(void) {
    std::lock_guard<std::mutex> lock(pixDataMutex_);

    float eMax = 0.0f;

    //  horizontal derivative
    for (auto y=0u; y<height_; ++y) {
        for (auto x=0u; x<width_; ++x) {
            auto& p = pixData_[y][x];
            auto& pn = pixData_[y][x+1];

            if (x == width_-1)
                p.dx = pixData_[y][x-1].dx;
            else
                p.dx = ((pn.c / (normConst_*pn.w)) - (p.c / (normConst_*p.w))).cast<float>();
        }
    }

    //  vertical derivative
    for (auto y=0u; y<height_; ++y) {
        for (auto x=0u; x<width_; ++x) {
            auto& p = pixData_[y][x];
            auto& pn = pixData_[y+1][x];

            if (y == height_-1)
                p.dy = pixData_[y-1][x].dy;
            else
                p.dy = ((pn.c / (normConst_*pn.w)) - (p.c / (normConst_*p.w))).cast<float>();

            p.e = p.dx.cwiseAbs().maxCoeff() + p.dy.cwiseAbs().maxCoeff();
            if (p.e > eMax) eMax = p.e;
        }
    }

    //  error value normalization
    for (auto y=0u; y<height_; ++y) {
        for (auto x=0u; x<width_; ++x) {
            auto& p = pixData_[y][x];

            p.e /= eMax;
        }
    }
}

float Canvas::getSamplingProbability(unsigned x, unsigned y) {
    std::lock_guard<std::mutex> lock(pixDataMutex_);

    return pixData_[y][x].e;
}
