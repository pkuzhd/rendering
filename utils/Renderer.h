//
// Created by zhd on 2022/7/6.
//

#ifndef RENDERING_RENDERER_H
#define RENDERING_RENDERER_H

#include <glad/glad.h>

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

    Renderer(GLuint FBO);

    ~Renderer();

    void createFramebuffers(int width, int height);

};


#endif //RENDERING_RENDERER_H
