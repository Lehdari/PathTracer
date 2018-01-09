#include "Scene.hpp"

#include <cstdio>
#include <map>
#include <memory>
#include <iostream> //  TEMP


Scene::Scene(void) :
    bvh_    (triangles_, Bvh::METRIC_OBJECT_MEDIAN)
{
}

Scene::Scene(const std::vector<Triangle>& triangles) :
    triangles_  (triangles),
    bvh_        (triangles_, Bvh::METRIC_OBJECT_MEDIAN)
{
}

void Scene::loadFromObj(const std::string& fileName) {
    std::lock_guard<std::mutex> lock(triangleMutex_);
    vertices_.clear();
    triangles_.clear();
    //normals_.clear();
    //indices_.clear();

    FILE* f;
    char* buffer = nullptr;
    std::unique_ptr<char> bufferUniq(nullptr);
    f = fopen(fileName.c_str(), "rb");

    if (f) {
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);

        bufferUniq.reset(new char[fsize+1]);
        buffer = bufferUniq.get();
        fread(buffer, fsize, 1, f);
        fclose(f);

        buffer[fsize] = '\0';
    }

    char lineHeader[32];
    std::vector<Vector3f> positions;
    std::vector<Vector2f> texCoords;
    std::vector<Vector3f> normals;
    std::vector<std::array<unsigned, 9>> indices;

    float dump;

    while(*buffer) {
        if (sscanf(buffer, "%s", lineHeader) == 0)
            return;

        buffer += strlen(lineHeader)+1;

        if (strcmp(lineHeader, "v") == 0) {
            Vector3f position{0.0f, 0.0f, 0.0f};
            if (sscanf(buffer, "%f %f %f %f", &position[0], &position[1], &position[2], &dump) < 3)
                throw "VertexData: invalid file!"; // TODO_EXCEPTION
            positions.push_back(std::move(position));
        }
        else if (strcmp(lineHeader, "vt") == 0) {
            Vector2f texCoord{0.0f, 0.0f};
            if (sscanf(buffer, "%f %f %f", &texCoord[0], &texCoord[1], &dump) < 2)
                throw "VertexData: invalid file!"; // TODO_EXCEPTION
            texCoords.push_back(std::move(texCoord));
        }
        else if (strcmp(lineHeader, "vn") == 0) {
            Vector3f normal{0.0f, 0.0f, 0.0f};
            if (sscanf(buffer, "%f %f %f", &normal[0], &normal[1], &normal[2]) < 3)
                throw "VertexData: invalid file!"; // TODO_EXCEPTION
            normals.push_back(std::move(normal));
        }
        else if (strcmp(lineHeader, "f") == 0) {
            std::array<unsigned, 9> index = {0, 0, 0,  0, 0, 0,  0, 0, 0};
            if (sscanf(buffer, "%u %u %u", &index[0], &index[1], &index[2]) == 3 ||
                sscanf(buffer, "%u//%u %u//%u %u//%u", &index[0], &index[6], &index[1], &index[7], &index[2], &index[8]) == 6 ||
                sscanf(buffer, "%u/%u %u/%u %u/%u", &index[0], &index[3], &index[1], &index[4], &index[2], &index[5]) == 6 ||
                sscanf(buffer, "%u/%u/%u %u/%u/%u %u/%u/%u", &index[0], &index[3], &index[6], &index[1], &index[4], &index[7], &index[2], &index[5], &index[8]) == 9)
                indices.push_back(std::move(index));
            else
                throw "VertexData: invalid file!"; // TODO_EXCEPTION
        }

        while (*buffer++ != 10)
            if (*buffer == 0)
                break;
    }

    std::map<std::array<unsigned, 3>, unsigned> createdVertexIds;
    std::vector<std::array<unsigned, 3>> triangleVertexIds;

    usingTexCoords_ = texCoords.size() > 0;
    usingNormals_ = normals.size() > 0;
    usingIndexing_ = true;

    for (auto& indexArray : indices) {
        if (usingTexCoords_ && (indexArray[3] == 0 || indexArray[4] == 0 || indexArray[5] == 0))
            throw "VertexData: invalid index data (texture coordinates)";

        if (usingNormals_ && (indexArray[6] == 0 || indexArray[7] == 0 || indexArray[8] == 0))
            throw "VertexData: invalid index data (normals)";

        std::array<unsigned, 3> v1 = { indexArray[0], indexArray[3], indexArray[6] };
        std::array<unsigned, 3> v2 = { indexArray[1], indexArray[4], indexArray[7] };
        std::array<unsigned, 3> v3 = { indexArray[2], indexArray[5], indexArray[8] };

        if (createdVertexIds[v1] == 0) {
            Vertex v{ {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

            v.p = positions.at(indexArray[0]-1);
            if (usingTexCoords_)
                v.t = texCoords.at(indexArray[3]-1);
            if (usingNormals_)
                v.n = normals.at(indexArray[6]-1);

            vertices_.push_back(std::move(v));
            createdVertexIds[v1] = vertices_.size()-1;
        }

        if (createdVertexIds[v2] == 0) {
            Vertex v{ {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

            v.p = positions.at(indexArray[1]-1);
            if (usingTexCoords_)
                v.t = texCoords.at(indexArray[4]-1);
            if (usingNormals_)
                v.n = normals.at(indexArray[7]-1);

            vertices_.push_back(std::move(v));
            createdVertexIds[v2] = vertices_.size()-1;
        }

        if (createdVertexIds[v3] == 0) {
            Vertex v{ {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

            v.p = positions.at(indexArray[2]-1);
            if (usingTexCoords_)
                v.t = texCoords.at(indexArray[5]-1);
            if (usingNormals_)
                v.n = normals.at(indexArray[8]-1);

            vertices_.push_back(std::move(v));
            createdVertexIds[v3] = vertices_.size()-1;
        }

        triangleVertexIds.push_back({createdVertexIds[v1], createdVertexIds[v2], createdVertexIds[v3]});
    }

    triangles_.reserve(indices.size());
    for (auto& ids : triangleVertexIds)
        triangles_.push_back({ &vertices_.at(ids[0]), &vertices_.at(ids[1]), &vertices_.at(ids[2]) });

    //  build BVH
    bvh_.build();
}

Hit Scene::traceRay(Ray& ray) {
    //std::lock_guard<std::mutex> lock(triangleMutex_);

    Hit hit;

    bvh_.traceRay(ray, hit);

#if 0
    for (auto& tri : triangles_) {
        Hit newHit = Hit::intersectRay(ray, tri);

        if (newHit.triangle())
            hit = newHit;
    }
#endif

    return hit;
}

const std::vector<Triangle>& Scene::getTriangles(void) {
    std::lock_guard<std::mutex> lock(triangleMutex_);
    return triangles_;
}

const std::vector<std::unique_ptr<Light>>& Scene::getLights(void) {
    return lights_;
}