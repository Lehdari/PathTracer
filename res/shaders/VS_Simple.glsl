#version 420 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 normal;

out vec4 pos;
out vec4 norm;
out vec3 col;

uniform mat4 MVP;
uniform vec3 Color;

void main() {
    pos = MVP * vec4(position, 1.0);
    norm = MVP * vec4(normal, 0.0);
    col = Color;
    gl_Position = pos;
}
