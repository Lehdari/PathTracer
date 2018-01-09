#ifndef BVH_HPP
#define BVH_HPP


#include "Triangle.hpp"

#include <vector>
#include <memory>


class Bvh {
public:
    //  enumerator for BVH build metric
    enum Metric {
        METRIC_UNKNOWN,
        METRIC_OBJECT_MEDIAN
    };

    //  wrapper struct for triangle pointer and metadata
    struct Object {
        const Triangle* triangle;
        uint64_t        triangleId;
        Vector3f        min;
        Vector3f        max;
        Vector3f        middle;
    };


    class Node {
    public:
        Node(void);

        void addTriangle(const std::vector<Triangle>& triangles, uint64_t id);
        void addObject(Object& object);
        void split(const Bvh::Metric& metric);
        //  note: updates triangle ids, call updateTrianglePointers immediately after
        void constructBalancedVector(std::vector<Triangle>& triangles);
        void updateTrianglePointers(const std::vector<Triangle>& triangles);

    private:
        std::vector<Object>     objects_;
        std::unique_ptr<Node>   children_[2];

        Vector3f                min_;
        Vector3f                max_;
    };


    Bvh(std::vector<Triangle>& triangles,
        const Metric& metric = METRIC_UNKNOWN);

    void build(void);

private:
    std::vector<Triangle>&  triangles_;
    Metric                  metric_;
    Node                    root_;
};


#endif // BVH_HPP
