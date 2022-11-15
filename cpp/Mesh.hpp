#include <iostream>

#include <vector>

#include <GL/glew.h>

#include "Loader.hpp"

class Mesh{
    public:
    float vertex[10000];
    int indices[10000];
    int totalv;
    void createMesh(const char* path){
        loadObj(path, vertex, indices, totalv);
        /*
        glGenTextures(1, &MeshTexture);
        glBindTexture(GL_TEXTURE_1D, MeshTexture);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        std::cout << "Total Vertices:" << vertex.size()/9 << std::endl;
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, vertex.size()/9, 0, GL_RGB, GL_FLOAT, vertex.data()); 
        glBindTexture(GL_TEXTURE_1D, MeshTexture);
        */
    }
};