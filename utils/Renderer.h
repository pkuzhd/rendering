//
// Created by zhd on 2022/7/6.
//

#ifndef RENDERING_RENDERER_H
#define RENDERING_RENDERER_H

#include <glad/glad.h>
#include <string>
#include "learnopengl/shader_m.h"

class Renderer {
public:
    GLint FBO;

    GLuint cameraFBO;
    GLuint cameraTexture;
    GLuint cameraDepth;

    GLuint accumulateFBO;
    GLuint accumulateTexture;

    GLuint screenVBO;
    GLuint screenVAO;

    GLuint backgroundVBO;
    GLuint backgroundVAO;

    GLuint foregroundVBO;
    GLuint foregroundVAO;

    GLuint background_texture;

    double *faces = nullptr;
    int num_face;

    enum PROGRAM {
        foreground,
        background,
        update,
        resolve,
    };

    Shader *foregroundProgram = nullptr;
    Shader *backgroundProgram= nullptr;
    Shader *updateProgram= nullptr;
    Shader *resolveProgram= nullptr;


    Renderer(GLuint FBO);

    ~Renderer();

    void createFramebuffers(int width, int height);

    void loadBackground(std::string back_file, std::string texture_file);

    void loadForegroundMesh(std::string mesh_file);

    void loadForeground();

    void clearBuffer();

    void renderBackground(GLuint show_type);

    void renderForegroundMesh(GLuint show_type, int *cam_select);

    void renderForeground(GLuint show_type);

    void renderBuffer();

    void createProgram(PROGRAM type, std::string vertexPath, std::string fragmentPath);

    void setView(glm::mat4 projection, glm::mat4 view);

};


#endif //RENDERING_RENDERER_H
