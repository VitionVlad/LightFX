#include <iostream>

#include <fstream>

#include <string>

#include "Mesh.hpp"

#include "Vectors.hpp"

#include "Shader.hpp"

#include <math.h>

#include <GLFW/glfw3.h>

void GLAPIENTRY MessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam){
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ), type, severity, message );
}

static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
};

class Engine{
    private:
    void printError(){
        std::cout << glGetError() << std::endl;
    }
    GLuint vertexbuffer;
    GLuint VertexArrayID;
    dvec2 mousepos;
    void loadCFG(){
        std::fstream cfg;
        cfg.open("App/config.cfg");
        std::string param{};
        int value{};
        while(cfg >> param >> value){
            if(param == "resx"){
                resx = value;
            }
            if(param == "resy"){
                resy = value;
            }
            if(param == "rresx"){
                rresx = value;
            }
            if(param == "rresy"){
                rresy = value;
            }
            if(param == "RPP"){
                rayPerPixel = value;
            }
            if(param == "RTC"){
                rtCount = value;
            }
        }
        cfg.close();
    }
    public:
    std::vector<float> vertex;
    std::vector<int> indices;
    int totalv = 0;
    GLFWwindow* window;
    int resx = 640;
    int resy = 480;
    int rresx = 100;
    int rresy = 100;
    vec3 position;
    vec2 rotation;
    float speed = 0.1;
    int sensivity = 2000;
    GLuint prog;
    GLuint postprog;
    GLuint FBO;
    GLuint RBO;
    GLuint MainColorTex;
    int rayPerPixel = 1;
    int rtCount = 2;
    void Init(){
        loadCFG();
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        window = glfwCreateWindow(resx, resy, "LightFX", NULL, NULL);
        glfwMakeContextCurrent(window);
        glewExperimental = true;
        glewInit();
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_TEXTURE_2D);
        glDebugMessageCallback(MessageCallback, 0);
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);
        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

        GLuint vshader = shader("App/Shaders/Main.vert", GL_VERTEX_SHADER);
        GLuint fshader = shader("App/Shaders/Main.frag", GL_FRAGMENT_SHADER);
        prog = glCreateProgram();
        glAttachShader(prog, vshader);
        glAttachShader(prog, fshader);
        glLinkProgram(prog);

        GLchar log[2000];
        glGetProgramInfoLog(prog, 2000, NULL, log);
        printf("%s\n", log);

        glDetachShader(prog, vshader);
        glDetachShader(prog, fshader);

        GLuint postvshader = shader("App/Shaders/Main.vert", GL_VERTEX_SHADER);
        GLuint postfshader = shader("App/Shaders/Post.frag", GL_FRAGMENT_SHADER);
        postprog = glCreateProgram();
        glAttachShader(postprog, postvshader);
        glAttachShader(postprog, postfshader);
        glLinkProgram(postprog);

        glGetProgramInfoLog(postprog, 2000, NULL, log);
        printf("%s\n", log);

        glDetachShader(postprog, postvshader);
        glDetachShader(postprog, postfshader); 

        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        glGenTextures(1, &MainColorTex);
        glBindTexture(GL_TEXTURE_2D, MainColorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rresx, rresy, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, MainColorTex, 0);

        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, rresx, rresy);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

        glActiveTexture(GL_TEXTURE0);
        glUseProgram(prog);
    }
    void applyMesh(Mesh& object, int begpos){
        vertex.resize(vertex.size()+object.totalv*3+begpos);
        indices.resize(vertex.size()+object.totalv*3+begpos);
        for(int i = 0; i != object.totalv*3; i++){
            vertex[i+begpos] = object.vertex[i];
            indices[i+begpos] = object.indices[i]+begpos;
        }
        totalv+=object.totalv;
    }
    void Update(GLFWkeyfun key_callback){
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1, 0.2, 0.3, 1.0);
        glfwGetFramebufferSize(window, &resx, &resy);
        glViewport(0, 0, rresx, rresy);
        glUseProgram(prog);

        glfwSetKeyCallback(window, key_callback);
        glfwGetCursorPos(window, &mousepos.x, &mousepos.y);
        rotation.x = mousepos.x / sensivity;
        rotation.y = mousepos.y / sensivity;

        glUniform3fv(glGetUniformLocation(prog, "Mesh"), totalv, vertex.data());
        glUniform3iv(glGetUniformLocation(prog, "MeshIndices"), totalv, indices.data());
        glUniform1i(glGetUniformLocation(prog, "totalv"), totalv);

        glUniform3f(glGetUniformLocation(prog, "position"), position.x, position.y, position.z);
        glUniform2f(glGetUniformLocation(prog, "rotation"), rotation.x, rotation.y);
        glUniform2f(glGetUniformLocation(prog, "resolution"), resx, resy);
        glUniform4f(glGetUniformLocation(prog, "random"), rand(), rand(), rand(), rand());
        glUniform2i(glGetUniformLocation(prog, "Settings"), rayPerPixel, rtCount);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, resx, resy);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1, 0.2, 0.3, 1.0);
        glUseProgram(postprog);

        glBindTexture(GL_TEXTURE_2D, MainColorTex);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    void Terminate(){  
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};