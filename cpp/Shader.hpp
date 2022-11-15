#include <iostream>

#include <cstring>

#include <GL/glew.h>

char *LoadFile(const char* path){
    static int len = 8000;
    char *text = (char*)malloc(len);
    memset(text, 0, len);
    FILE *f;
    f = fopen(path, "rb");
    if(f){
        fread(text, 1, len, f);
        fclose(f);
    }
    return text;
}

GLuint shader(const char* path, GLenum type){
    char *txt = LoadFile(path);
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &txt, NULL);
    glCompileShader(sh);
    free(txt);
    GLchar log[2000];
    glGetShaderInfoLog(sh, 2000, NULL, log);
    printf("%s\n", log);
    return sh;
}