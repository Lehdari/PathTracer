#version 420 core

in vec4 pos;

out vec4 color;

uniform sampler2D sampler;

void main() {
    color = vec4(texture(sampler, pos.xy*0.5 + vec2(0.5)).xyz, 1.0);
}
