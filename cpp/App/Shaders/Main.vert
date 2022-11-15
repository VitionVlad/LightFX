#version 400 core

layout(location = 0) in vec3 positions;

out vec2 uv;

void main(){
    gl_Position = vec4(positions, 1);
    vec2 finuv = positions.xy;
    if(finuv.x == -1.0){
        finuv.x = 0.0;
    }
    if(finuv.y == -1.0){
        finuv.y = 0.0;
    }
    uv = finuv;
}