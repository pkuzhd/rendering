//
// Created by zhd on 2022/7/6.
//

#include "Renderer.h"

Renderer::Renderer(GLuint FBO) : FBO(FBO) {
    cameraFBO = cameraTexture = cameraDepth = 0;
    accumulateFBO = accumulateTexture = 0;

    const float tex[3][2] = {
            {0, 0},
            {0, 2},
            {2, 0},
    };
    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);

    glBindVertexArray(screenVAO);

    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, tex, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(0);
}

Renderer::~Renderer() {
//    glDeleteVertexArrays(1, &screenVAO);
//    glDeleteBuffers(1, &screenVBO);

//    glDeleteFramebuffers(1, &cameraFBO);
}

void Renderer::createFramebuffers(int width, int height) {
    glGenFramebuffers(1, &cameraFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, cameraFBO);

    glGenTextures(1, &cameraTexture);
    glBindTexture(GL_TEXTURE_2D, cameraTexture);
    glTexImage2D(
            GL_TEXTURE_2D,
            0, // level
            GL_RGBA32F,
            width,
            height,
            0, // border must be 0
            GL_RGBA,
            GL_BYTE,
            nullptr); // no pixel data
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cameraTexture, 0);

    glGenRenderbuffers(1, &cameraDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, cameraDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, cameraDepth);

    glGenFramebuffers(1, &accumulateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, accumulateFBO);

    glGenTextures(1, &accumulateTexture);
    glBindTexture(GL_TEXTURE_2D, accumulateTexture);
    glTexImage2D(
            GL_TEXTURE_2D,
            0, // level
            GL_RGBA32F,
            width,
            height,
            0, // border must be 0
            GL_RGBA,
            GL_BYTE,
            nullptr); // no pixel data
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumulateTexture, 0);
}