#version 450 core

layout(location = 0)in vec2 vertPos;

uniform mat4 mat;

void main(){
    gl_Position = mat*vec4(vertPos,0.0,1.0);
}