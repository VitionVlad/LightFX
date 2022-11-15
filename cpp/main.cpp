#include <iostream>

#include "Engine.hpp"

using namespace std;

Engine eng;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_W && action == GLFW_REPEAT) {
        eng.position.x += cos(eng.rotation.y) * cos(eng.rotation.x) * eng.speed;
        eng.position.y += cos(eng.rotation.y) * sin(eng.rotation.x) * eng.speed;
    }
    if (key == GLFW_KEY_A && action == GLFW_REPEAT) {
        eng.position.y -= cos(eng.rotation.y) * cos(eng.rotation.x) * eng.speed;
        eng.position.x += cos(eng.rotation.y) * sin(eng.rotation.x) * eng.speed;
    }
    if (key == GLFW_KEY_S && action == GLFW_REPEAT) {
        eng.position.x -= cos(eng.rotation.y) * cos(eng.rotation.x) * eng.speed;
        eng.position.y -= cos(eng.rotation.y) * sin(eng.rotation.x) * eng.speed;
    }
    if (key == GLFW_KEY_D && action == GLFW_REPEAT) {
        eng.position.y += cos(eng.rotation.y) * cos(eng.rotation.x) * eng.speed;
        eng.position.x -= cos(eng.rotation.y) * sin(eng.rotation.x) * eng.speed;
    }
    if (key == GLFW_KEY_Q && action == GLFW_REPEAT) {
        eng.position.z += eng.speed;
    }
    if (key == GLFW_KEY_E && action == GLFW_REPEAT) {
        eng.position.z -= eng.speed;
    }
}

int main(){
    eng.Init();
    glfwSetInputMode(eng.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Mesh object;

    object.createMesh("App/Models/untitled.obj");

    eng.applyMesh(object, 0);

    while(!glfwWindowShouldClose(eng.window)){
        eng.Update(key_callback);
    }
    eng.Terminate();
}