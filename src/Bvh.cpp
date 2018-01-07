#include "Bvh.hpp"


#define MIN(A, B, C) std::min(A, std::min(B, C))
#define MAX(A, B, C) std::max(A, std::max(B, C))


Bvh::Bvh(std::vector<Triangle>& triangles, const Metric& metric) :
    triangles_  (triangles),
    metric_     (metric)
{
    if (metric_ == METRIC_UNKNOWN) {
        fprintf(stderr, "ERROR: Unknown BVH metric, aborting BVH build.\n");
        return;
    }
}

void Bvh::build(void)
{
    printf("Building BVH...\n");

    for (auto& t : triangles_) {
        root_.addTriangle(t);
    }

    root_.split(metric_);

    printf("done.\n");
}

Bvh::Node::Node(void) :
    min_(FLT_MAX, FLT_MAX, FLT_MAX),
    max_(FLT_MIN, FLT_MIN, FLT_MIN)
{
}

void Bvh::Node::addTriangle(Triangle& triangle)
{
    objects_.emplace_back();
    auto& o = objects_.back();
    o.triangle = &triangle;

    auto& v1 = triangle.v[0]->p;
    auto& v2 = triangle.v[1]->p;
    auto& v3 = triangle.v[2]->p;

    //  calculate middle point
    o.middle = (v1 + v2 + v3) * 0.33333333f;

    //  calculate node boundaries
    o.min = Vector3f(MIN(v1(0), v2(0), v3(0)),
                         MIN(v1(1), v2(1), v3(1)),
                         MIN(v1(2), v2(2), v3(2)));
    o.max = Vector3f(MAX(v1(0), v2(0), v3(0)),
                         MAX(v1(1), v2(1), v3(1)),
                         MAX(v1(2), v2(2), v3(2)));

    if (o.min(0) < min_(0)) min_(0) = o.min(0);
    if (o.min(1) < min_(1)) min_(1) = o.min(1);
    if (o.min(2) < min_(2)) min_(2) = o.min(2);
    if (o.max(0) > max_(0)) max_(0) = o.max(0);
    if (o.max(1) > max_(1)) max_(1) = o.max(1);
    if (o.max(2) > max_(2)) max_(2) = o.max(2);
}

void Bvh::Node::addObject(Object& object)
{
    objects_.push_back(object);
    auto& o = objects_.back();

    if (o.min(0) < min_(0)) min_(0) = o.min(0);
    if (o.min(1) < min_(1)) min_(1) = o.min(1);
    if (o.min(2) < min_(2)) min_(2) = o.min(2);
    if (o.max(0) > max_(0)) max_(0) = o.max(0);
    if (o.max(1) > max_(1)) max_(1) = o.max(1);
    if (o.max(2) > max_(2)) max_(2) = o.max(2);
}

void Bvh::Node::split(const Bvh::Metric& metric)
{
    printf("Splitting node...\n");

    auto n = objects_.size();
    auto h = n/2;

    if (n < 2) {
        printf("asd\n");
        return;
    }

    switch (metric) {
    case Bvh::METRIC_OBJECT_MEDIAN: {
        //  sort along x using middle point
        std::sort(objects_.begin(), objects_.end(), [](const Object& o1, const Object& o2) {
            return o1.middle(0) < o2.middle(0);
        });

        children_[0] = std::make_unique<Node>();
        children_[1] = std::make_unique<Node>();

        for (decltype(n) i=0; i<h; ++i)
            children_[0]->addObject(objects_[i]);

        for (decltype(n) i=h; i<n; ++i)
            children_[1]->addObject(objects_[i]);

        objects_.clear();

        break;
    }
    default:
        fprintf(stderr, "ERROR: Cannot split BVH node, unknown metric.\n");
        break;
    }

    children_[0]->split(metric);
    children_[1]->split(metric);
}
