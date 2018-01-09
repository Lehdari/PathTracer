#include "Bvh.hpp"

#include <iostream> //  TEMP


#define MIN(A, B, C) std::min(A, std::min(B, C))
#define MAX(A, B, C) std::max(A, std::max(B, C))


namespace {

    inline float surfaceArea(const Vector3f& min, const Vector3f& max)
    {
        auto d = max-min;
        return 2.0f * (d(0)*d(1) + d(0)*d(2) + d(1)*d(2));
    }

}


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

    for (auto i=0llu; i<triangles_.size(); ++i)
        root_.addTriangle(triangles_, i);

    root_.split(metric_);

    printf("done.\n");
}

Bvh::Node::Node(void) :
    min_(FLT_MAX, FLT_MAX, FLT_MAX),
    max_(-FLT_MAX, -FLT_MAX, -FLT_MAX)
{
}

void Bvh::Node::addTriangle(const std::vector<Triangle>& triangles, uint64_t id)
{
    auto& triangle = triangles[id];
    objects_.emplace_back();
    auto& o = objects_.back();
    o.triangle = &triangle;
    o.triangleId = id;

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
        std::vector<Object> oTemp[3] = {objects_, objects_, objects_};
        Node n1[3], n2[3];
        float area[3] = {0.0f};
        int minArea = 0;

        for (auto i=0; i<3; ++i) {
            std::sort(oTemp[i].begin(), oTemp[i].end(), [&i](const Object& o1, const Object& o2) -> bool {
                return o1.middle(i) < o2.middle(i);
            });

            for (decltype(n) j = 0; j < h; ++j)
                n1[i].addObject(oTemp[i][j]);
            for (decltype(n) j = h; j < n; ++j)
                n2[i].addObject(oTemp[i][j]);

            auto min1 = n1[i].min_;
            auto max1 = n1[i].max_;
            auto min2 = n2[i].min_;
            auto max2 = n2[i].max_;

            auto s1 = max1-min1;
            auto s2 = max2-min2;

            float a1 = surfaceArea(min1, max1);
            float a2 = surfaceArea(min2, max2);

            printf("Child 1 [%d]:\n", i);
            //std::cout << min1.transpose() << std::endl;
            //std::cout << max1.transpose() << std::endl;
            //std::cout << s1.transpose() << std::endl;
            printf("Surface area: %0.4f\n", a1);


            printf("Child 2 [%d]:\n", i);
            //std::cout << min2.transpose() << std::endl;
            //std::cout << max2.transpose() << std::endl;
            //std::cout << s2.transpose() << std::endl;
            printf("Surface area: %0.4f\n", a2);
            printf("Total surface area: %0.4f\n", a1+a2);

            area[i] = a1+a2;
            if (area[i] < area[minArea])
                minArea = i;
        }

        printf("minArea: %d\n", minArea);

        children_[0] = std::make_unique<Node>();
        children_[1] = std::make_unique<Node>();

        for (decltype(n) i = 0; i < h; ++i)
            children_[0]->addObject(oTemp[minArea][i]);

        for (decltype(n) i = h; i < n; ++i)
            children_[1]->addObject(oTemp[minArea][i]);

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