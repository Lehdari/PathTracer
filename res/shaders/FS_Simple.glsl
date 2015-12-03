#version 420 core

varying vec4 pos;
varying vec4 norm;
varying vec3 col;

out vec4 color;

void main() {
    float shade = 0.6+0.4*dot(normalize(vec3(-1.0f, -1.0f, -2.0f)), normalize(norm.xyz));
    color = vec4(col * shade, 1.0);
    //color = vec3(0.5)+norm.xyz*0.5;
    //color = vec4(vec3(gl_FragCoord.z), 1.0f);
}
