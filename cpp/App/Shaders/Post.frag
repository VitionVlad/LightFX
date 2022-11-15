#version 400 core

in vec2 uv;

out vec4 color;

uniform sampler2D MainFrame;

void main(){
    color = vec4(texture(MainFrame, uv).rgb, 1.0);
}