#include "Mesh.hpp"

#include <vector>
#include <array>
#include <map>
#include <memory>
#include <cstdio>
#include <cstring>
#include <cctype>


Mesh::Mesh(void) :
    vertexArrayObjectId_(0),
    positionBufferId_(0),
    texCoordBufferId_(0),
    normalBufferId_(0),
    elementBufferId_(0),
    position_(0.0f, 0.0f, 0.0f),
    rotation_(Matrix3Glf::Identity())
{}

Mesh::~Mesh(void) {
    if (vertexArrayObjectId_)
        glDeleteVertexArrays(1, &vertexArrayObjectId_);
    if (positionBufferId_)
        glDeleteBuffers(1, &positionBufferId_);
    if (normalBufferId_)
        glDeleteBuffers(1, &normalBufferId_);
    if (texCoordBufferId_)
        glDeleteBuffers(1, &texCoordBufferId_);
    if (elementBufferId_)
        glDeleteBuffers(1, &elementBufferId_);
}

void Mesh::loadFromObj(const std::string& fileName) {
    std::vector<std::array<float, 4>> positions;
    std::vector<std::array<float, 3>> texCoords;
    std::vector<std::array<float, 3>> normals;
    std::vector<unsigned> indices;

    FILE* f = fopen(fileName.c_str(), "rb");
    if (!f)
        throw "File could not be opened!"; // TODO_EXCEPTION

    fseek(f, 0L, SEEK_END);
    auto fs = ftell(f);
    std::unique_ptr<char> bufUnique(new char[fs]);
    char* buffer = bufUnique.get();
    fseek(f, 0L, SEEK_SET);
    fread(buffer, sizeof(char), fs, f);
    fclose(f);

    char lineHeader[32];
    std::vector<std::array<float, 4>> tempPositions;
    std::vector<std::array<float, 3>> tempTexCoords;
    std::vector<std::array<float, 3>> tempNormals;
    std::vector<std::array<unsigned, 9>> tempIndexArrays;

    while(*buffer) {
        if (isprint(*buffer)) {
            if (sscanf(buffer, "%s", lineHeader) == 0)
                return;
        }
        else {
            ++buffer;
            continue;
        }

        buffer += strlen(lineHeader)+1;

        if (strcmp(lineHeader, "v") == 0) {
            std::array<float, 4> position = {0.0f, 0.0f, 0.0f, 1.0f};
            if (sscanf(buffer, "%f %f %f %f", &position[0], &position[1], &position[2], &position[3]) < 3)
                throw "VertexData: invalid file!"; // TODO_EXCEPTION
            tempPositions.push_back(std::move(position));
        }
        else if (strcmp(lineHeader, "vt") == 0) {
            std::array<float, 3> texCoord = {0.0f, 0.0f, 0.0f};
            if (sscanf(buffer, "%f %f %f", &texCoord[0], &texCoord[1], &texCoord[2]) < 2)
                throw "VertexData: invalid file!"; // TODO_EXCEPTION
            tempTexCoords.push_back(std::move(texCoord));
        }
        else if (strcmp(lineHeader, "vn") == 0) {
            std::array<float, 3> normal = {0.0f, 0.0f, 0.0f};
            if (sscanf(buffer, "%f %f %f", &normal[0], &normal[1], &normal[2]) < 3)
                throw "VertexData: invalid file!"; // TODO_EXCEPTION
            tempNormals.push_back(std::move(normal));
        }
        else if (strcmp(lineHeader, "f") == 0) {
            std::array<unsigned, 9> index = {0, 0, 0,  0, 0, 0,  0, 0, 0};
            if (sscanf(buffer, "%u %u %u", &index[0], &index[1], &index[2]) == 3 ||
                sscanf(buffer, "%u//%u %u//%u %u//%u", &index[0], &index[6], &index[1], &index[7], &index[2], &index[8]) == 6 ||
                sscanf(buffer, "%u/%u %u/%u %u/%u", &index[0], &index[3], &index[1], &index[4], &index[2], &index[5]) == 6 ||
                sscanf(buffer, "%u/%u/%u %u/%u/%u %u/%u/%u", &index[0], &index[3], &index[6], &index[1], &index[4], &index[7], &index[2], &index[5], &index[8]) == 9)
                tempIndexArrays.push_back(std::move(index));
            else
                throw "VertexData: invalid file!"; // TODO_EXCEPTION
        }

        while (*buffer++ != 10)
            if (*buffer == 0)
                break;
    }

    std::map<std::array<unsigned, 3>, unsigned> createdVertices;

    bool usingTexCoords = tempTexCoords.size() > 0;
    bool usingNormals = tempNormals.size() > 0;
    bool usingIndexing = true;

    for (auto& indexArray : tempIndexArrays) {
        if (usingTexCoords && (indexArray[3] == 0 || indexArray[4] == 0 || indexArray[5] == 0))
            throw "VertexData: invalid index data (texture coordinates)";

        if (usingNormals && (indexArray[6] == 0 || indexArray[7] == 0 || indexArray[8] == 0))
            throw "VertexData: invalid index data (normals)";

        std::array<unsigned, 3> v1 = { indexArray[0], indexArray[3], indexArray[6] };
        std::array<unsigned, 3> v2 = { indexArray[1], indexArray[4], indexArray[7] };
        std::array<unsigned, 3> v3 = { indexArray[2], indexArray[5], indexArray[8] };

        if (createdVertices[v1] == 0) {
            positions.push_back(tempPositions.at(indexArray[0]-1));
            if (usingTexCoords)
                texCoords.push_back(tempTexCoords.at(indexArray[3]-1));
            if (usingNormals)
                normals.push_back(tempNormals.at(indexArray[6]-1));

            createdVertices[v1] = positions.size()-1;
        }
        indices.push_back(createdVertices[v1]);

        if (createdVertices[v2] == 0) {
            positions.push_back(tempPositions.at(indexArray[1]-1));
            if (usingTexCoords)
                texCoords.push_back(tempTexCoords.at(indexArray[4]-1));
            if (usingNormals)
                normals.push_back(tempNormals.at(indexArray[7]-1));

            createdVertices[v2] = positions.size()-1;
        }
        indices.push_back(createdVertices[v2]);

        if (createdVertices[v3] == 0) {
            positions.push_back(tempPositions.at(indexArray[2]-1));
            if (usingTexCoords)
                texCoords.push_back(tempTexCoords.at(indexArray[5]-1));
            if (usingNormals)
                normals.push_back(tempNormals.at(indexArray[8]-1));

            createdVertices[v3] = positions.size()-1;
        }
        indices.push_back(createdVertices[v3]);
    }

    //  create and bind the VAO
    glGenVertexArrays(1, &vertexArrayObjectId_);
    glBindVertexArray(vertexArrayObjectId_);

    //  upload the vertex data to GPU and set up the vertex attribute arrays
    glGenBuffers(1, &positionBufferId_);
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferId_);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(std::array<float, 4>), &positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    if (usingTexCoords) {
        glGenBuffers(1, &texCoordBufferId_);
        glBindBuffer(GL_ARRAY_BUFFER, texCoordBufferId_);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(std::array<float, 3>), &texCoords[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    }

    if (usingNormals) {
        glGenBuffers(1, &normalBufferId_);
        glBindBuffer(GL_ARRAY_BUFFER, normalBufferId_);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(std::array<float, 3>), &normals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    }

    if (usingIndexing) {
        nIndices_ = indices.size();
        glGenBuffers(1, &elementBufferId_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices_ * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);
    }

    //  unbind the VAO so it won't be changed outside this function
    glBindVertexArray(0);
}

void Mesh::render(const Shader& shader, const BasicCamera& camera, const Vector3Glf& color) const {
    shader.useShader(camera.getVP() * getOrientation(), color);
    glBindVertexArray(vertexArrayObjectId_);

    glDrawElements(GL_TRIANGLES, nIndices_, GL_UNSIGNED_INT, (GLvoid*)0);

    glBindVertexArray(0);
}

void Mesh::setPosition(const Vector3Glf& position) {
    position_ = position;
}

void Mesh::setRotation(const Matrix3Glf& rotation) {
    rotation_ = rotation;
}

Matrix4Glf Mesh::getOrientation(void) const {
    Matrix4Glf o;
    o << rotation_          , position_ ,
         0.0f, 0.0f, 0.0f   , 1.0f      ;
    return std::move(o);
}
