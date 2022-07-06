//
// Created by zhd on 2022/7/6.
//

#include "Renderer.h"
#include "ply/PlyReader.h"
#include "ply/plyUtils.h"
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

using namespace std;

Renderer::Renderer(GLuint FBO) : FBO(FBO) {
    cameraFBO = cameraTexture = cameraDepth = 0;
    accumulateFBO = accumulateTexture = 0;

    glGenVertexArrays(1, &backgroundVAO);
    glGenBuffers(1, &backgroundVBO);


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
    delete[] faces;
    delete foregroundProgram;
    delete backgroundProgram;
    delete updateProgram;
    delete resolveProgram;
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

void Renderer::loadBackground(string back_file, string texture_file) {
    int faceIndexSize = 0;

    double *vertexArrayPLY, *coordArrayPLY;
    int vertexSize = 0;
    int faceCoordSize = 0;


    int vertexPropertySize = 3;
    int faceIndexPropertySize = 3;
    int faceCoordPorpertySize = 6;
    PlyReader *reader = initialReader(back_file);
    double *tmp = initialDoubleArray(reader, elementVertex, vertexPropertySize, &vertexSize);
    long *indexArray = initialLongArray(reader, elementFace, faceIndexPropertySize, &faceIndexSize);
    coordArrayPLY = initialDoubleArray(reader, elementFace, faceCoordPorpertySize, &faceCoordSize);

    assert(faceIndexSize == faceCoordSize);
    getArrayfromPly(reader, tmp, coordArrayPLY, indexArray);
    vertexArrayPLY = new double[faceIndexSize * faceIndexPropertySize * 3];
    for (int i = 0; i < faceIndexSize; ++i) {
        vertexArrayPLY[9 * i + 0] = tmp[3 * indexArray[3 * i + 0] + 0];
        vertexArrayPLY[9 * i + 1] = tmp[3 * indexArray[3 * i + 0] + 1];
        vertexArrayPLY[9 * i + 2] = tmp[3 * indexArray[3 * i + 0] + 2];
        vertexArrayPLY[9 * i + 3] = tmp[3 * indexArray[3 * i + 1] + 0];
        vertexArrayPLY[9 * i + 4] = tmp[3 * indexArray[3 * i + 1] + 1];
        vertexArrayPLY[9 * i + 5] = tmp[3 * indexArray[3 * i + 1] + 2];
        vertexArrayPLY[9 * i + 6] = tmp[3 * indexArray[3 * i + 2] + 0];
        vertexArrayPLY[9 * i + 7] = tmp[3 * indexArray[3 * i + 2] + 1];
        vertexArrayPLY[9 * i + 8] = tmp[3 * indexArray[3 * i + 2] + 2];
    }
    faces = new double[faceIndexSize * 5 * 3];
    memcpy(faces, vertexArrayPLY, sizeof(double) * faceIndexSize * 3 * 3);
    memcpy(faces + faceIndexSize * 3 * 3, coordArrayPLY, sizeof(double) * faceIndexSize * 2 * 3);
    delete[]tmp;
    delete[]indexArray;
    delete[]vertexArrayPLY;
    delete[]coordArrayPLY;

    num_face = faceIndexSize;

    glBindVertexArray(backgroundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * 3 * 5 * faceIndexSize, faces, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), (void *) 0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, 2 * sizeof(double),
                          (void *) (sizeof(double) * 3 * 3 * faceIndexSize));
    glEnableVertexAttribArray(1);

    glGenTextures(1, &background_texture);
    glBindTexture(GL_TEXTURE_2D, background_texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
//    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load(texture_file.c_str(), &width,
                                    &height,
                                    &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

void Renderer::loadForegroundMesh(std::string mesh_file) {
    int num_tri = 857132 * 2;
    float *vertices = new float[num_tri * 3 * 6];
    FILE *f = fopen(mesh_file.c_str(), "rb");
    fread(vertices, num_tri * 3 * 6 * sizeof(float), 1, f);
    fclose(f);

    glGenVertexArrays(1, &foregroundVAO);
    glGenBuffers(1, &foregroundVBO);

    glBindVertexArray(foregroundVAO);

    glBindBuffer(GL_ARRAY_BUFFER, foregroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 3 * num_tri, vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Renderer::clearBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, accumulateFBO);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::renderBackground(GLuint show_type) {
    // clearSubframe
    glBindFramebuffer(GL_FRAMEBUFFER, cameraFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    // renderSubframe
    backgroundProgram->use();
    glPolygonMode(GL_FRONT_AND_BACK, show_type);
    glBindVertexArray(backgroundVAO);
    glBindTexture(GL_TEXTURE_2D, background_texture);
    glEnable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 3 * num_face);

    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);

    // updateAccumulation
    updateProgram->use();
    glBindFramebuffer(GL_FRAMEBUFFER, accumulateFBO);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);

    glBindVertexArray(screenVAO);
    glBindTexture(GL_TEXTURE_2D, cameraTexture);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisable(GL_BLEND);
}

void Renderer::createProgram(PROGRAM type, std::string vertexPath, std::string fragmentPath) {
    switch (type) {
        case foreground:
            foregroundProgram = new Shader(vertexPath.c_str(), fragmentPath.c_str());
            break;
        case background:
            backgroundProgram = new Shader(vertexPath.c_str(), fragmentPath.c_str());
            break;
        case update:
            updateProgram = new Shader(vertexPath.c_str(), fragmentPath.c_str());
            break;
        case resolve:
            resolveProgram = new Shader(vertexPath.c_str(), fragmentPath.c_str());
            break;
    }
}

void Renderer::setView(glm::mat4 projection, glm::mat4 view) {
    backgroundProgram->use();
    backgroundProgram->setMat4("projection", projection);
    backgroundProgram->setMat4("view", view);
    backgroundProgram->setMat4("model", glm::mat4(1.0f));
    foregroundProgram->use();
    foregroundProgram->setMat4("projection", projection);
    foregroundProgram->setMat4("view", view);
    foregroundProgram->setMat4("model", glm::mat4(1.0f));
}

void Renderer::renderForegroundMesh(GLuint show_type, int *cam_select) {
    glm::vec3 centers[5] = {
            {-0.260775,            -0.208567,            0.013085},
            {-0.13337931097454817, -0.09792640328405332, 0.011925746660820238},
            {0,                    0,                    0},
            {0.13273071141398268,  -0.07897766327585604, -0.030115580768282482},
            {0.26841132121195244,  -0.16857177340133145, -0.05398189913034572},
    };

    float centers_f[5][3] = {
            {-0.260775,            -0.208567,            0.013085},
            {-0.13337931097454817, -0.09792640328405332, 0.011925746660820238},
            {0,                    0,                    0},
            {0.13273071141398268,  -0.07897766327585604, -0.030115580768282482},
            {0.26841132121195244,  -0.16857177340133145, -0.05398189913034572},
    };
    foregroundProgram->use();
    glUniform3fv(glGetUniformLocation(foregroundProgram->ID, "centers"), 5, reinterpret_cast<const GLfloat *>(centers_f));

    int tris[] = {0, 176124, 348068, 520032, 688200, 857132};

    for (int i = 0; i < 5; ++i) {
        // clearSubframe
        glBindFramebuffer(GL_FRAMEBUFFER, cameraFBO);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // renderSubframe
        foregroundProgram->use();
        foregroundProgram->setVec3("center", centers[i]);
        foregroundProgram->setInt("idx", i);
        glPolygonMode(GL_FRONT_AND_BACK, show_type);
        glBindVertexArray(foregroundVAO);
        glEnable(GL_DEPTH_TEST);
        if (cam_select[i])
            glDrawArrays(GL_TRIANGLES, 6 * tris[i], 6 * (tris[i + 1] - tris[i]));
        glDisable(GL_DEPTH_TEST);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // updateAccumulation
        updateProgram->use();
        glBindFramebuffer(GL_FRAMEBUFFER, accumulateFBO);
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);

        glBindVertexArray(screenVAO);
        glBindTexture(GL_TEXTURE_2D, cameraTexture);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glDisable(GL_BLEND);
    }
}


void Renderer::renderForeground(GLuint show_type) {

}

void Renderer::renderBuffer() {
    resolveProgram->use();
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(screenVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, accumulateTexture);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}





