#ifndef FILTER_HPP
#define FILTER_HPP


class Filter {
public:
    enum Type {
        TYPE_DIRACDELTA,    //  dirac-delta function
        TYPE_TENT,          //  tent function
        TYPE_GAUSSIAN       //  gaussian function
    };

    friend class Canvas;

    Filter(Type type, float width);

    void setWidth(float width);
    float getWidth(void) const;

    float operator()(float sampleX, float sampleY,
                     float x0, float x1, float y0, float y1) const;

    float operator()(float sampleX, float sampleY,
                     float x0, float x1, float y0, float y1, float width) const;

private:
    Type type_;
    float width_;
};


#endif // FILTER_HPP
