#version 420 core

layout(location = 0) in vec3 position;

out vec4 pos;

void main() {
    pos = vec4(position, 1.0);
    gl_Position = pos;
}
