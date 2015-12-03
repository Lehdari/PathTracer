/**

    Automaatio- ja Systeemitekniikan killan robokäsiprojekti
    VS_Joint.glsl

    @version    0.1
    @author     Miika 'LehdaRi' Lehtimäki
    @date       2015-04-18

**/


#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec3 col;

uniform mat4 MVP;
uniform vec3 Color;

void main() {
    col = color;
    gl_Position = MVP * vec4(position, 1.0);
}

