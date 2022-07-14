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
    int num_camera;
    glm::mat4 *K_invs;
    glm::mat4 *R_invs;
    int *widths, *heights;
    int M, N;

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
    GLuint foregroundEBO;

    GLuint background_texture;
    GLuint *rgb_textures;
    GLuint *depth_textures;
    GLuint *mask_textures;

    double *faces = nullptr;
    int num_face;

    enum PROGRAM {
        foreground,
        background,
        update,
        resolve,
    };

    Shader *foregroundProgram = nullptr;
    Shader *backgroundProgram = nullptr;
    Shader *updateProgram = nullptr;
    Shader *resolveProgram = nullptr;


    Renderer(GLuint FBO, int num_camera = 5);

    ~Renderer();

    void createFramebuffers(int width, int height);

    void loadBackground(std::string back_file, std::string texture_file);

    void loadForegroundMesh(std::string mesh_file);

    void loadForegroundFile(std::string path, int M, int N);

    void loadForegroundTexture(void *rgb, void *depth, void *mask, int cam_id, int width = -1, int height = -1);

    void clearBuffer();

    void renderBackground(GLuint show_type);

    void renderForegroundMesh(GLuint show_type, int *cam_select);

    void renderForegroundFile(GLuint show_type, int *cam_select);

    void renderBuffer();

    void createProgram(PROGRAM type, std::string vertexPath, std::string fragmentPath);

    void setView(glm::mat4 projection, glm::mat4 view);

    void setModel(glm::mat4 model);

};


#endif //RENDERING_RENDERER_H
