#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <jsoncpp/json/json.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <iostream>
#include <string>

#include "ply/PlyReader.h"
#include "ply/plyUtils.h"
#include "utils/Renderer.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using std::cout;
using std::endl;
using std::string;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f),
              glm::vec3(0.0f, 1.0f, 0.0f),
              YAW, PITCH);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

#define N 1920
#define M 1080

float w = 384.0f;
float h = 640.0f;

int pointSize = 1;
int cam_select[5] = {1, 1, 1, 1, 1};
glm::vec3 centers[5] = {
        {0.        ,  0.        ,  0.        },
        {0.13442005, -0.00617611,  0.00693867},
        {0.26451552, -0.01547954,  0.01613046},
        {0.39635817, -0.01958267,  0.02322004},
        {0.53632535, -0.02177305,  0.05059797}
};

GLuint show_type = GL_FILL;
GLuint cursor_type = GLFW_CURSOR_DISABLED;

void key_callback(GLFWwindow *window, const int key, const int s, const int action, const int mods) {
    if (action == GLFW_RELEASE)
        return;
    if (action == GLFW_PRESS) {
        if (key >= GLFW_KEY_1 && key <= GLFW_KEY_5)
            cam_select[key - GLFW_KEY_1] = 1 - cam_select[key - GLFW_KEY_1];
        if (key >= GLFW_KEY_6 && key <= GLFW_KEY_9)
            camera.Position = centers[key - GLFW_KEY_6];
        if (key == GLFW_KEY_0)
            camera.Position = centers[4];
        if (key == GLFW_KEY_MINUS)
            pointSize = std::max(1, pointSize - 1);
        if (key == GLFW_KEY_EQUAL)
            pointSize = std::min(10, pointSize + 1);
        if (key == GLFW_KEY_COMMA)
            cursor_type = cursor_type == GLFW_CURSOR_DISABLED ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
    }
}

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);


    GLint FBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &FBO);
//
    Renderer renderer(FBO);
    renderer.createProgram(Renderer::foreground, "./shaders/camera.vert", "./shaders/camera.frag");
    renderer.createProgram(Renderer::background, "./shaders/background.vert", "./shaders/background.frag");
    renderer.createProgram(Renderer::update, "./shaders/update.vert", "./shaders/update.frag");
    renderer.createProgram(Renderer::resolve, "./shaders/resolve.vert", "./shaders/resolve.frag");
    renderer.createFramebuffers(SCR_WIDTH, SCR_HEIGHT);
//
//    renderer.loadBackground("./data/scene_dense_mesh_refine_texture.ply", "./data/scene_dense_mesh_refine_texture.png");
    renderer.loadForegroundFile("./data/para.json", M, N);

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    int width, height, nrChannels;
    unsigned char *data;
    width = 384;
    height = 640;

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    renderer.foregroundProgram->use();
    renderer.foregroundProgram->setInt("rgb", 0);
    renderer.foregroundProgram->setInt("depth", 1);
    renderer.foregroundProgram->setInt("mask", 2);

    if (0) {
        string path = "./data/";
        for (int i = 0; i < renderer.num_camera; ++i) {
            int nrChannels;
            unsigned char *rgb = stbi_load((path + std::to_string(i + 1) + ".png").c_str(), &renderer.widths[i],
                                           &renderer.heights[i],
                                           &nrChannels, 0);
            renderer.widths[i] = 384;
            renderer.heights[i] = 640;

            float *depth = new float[renderer.widths[i] * renderer.heights[i]];
            FILE *f = fopen((path + std::to_string(i + 1) + ".depth2").c_str(), "rb");
            fread(depth, renderer.widths[i] * renderer.heights[i] * sizeof(float), 1, f);
            fclose(f);

            unsigned char *mask = new unsigned char[renderer.widths[i] * renderer.heights[i]];
            f = fopen((path + std::to_string(i + 1) + ".mask").c_str(), "rb");
            fread(mask, 640 * 384 * sizeof(unsigned char), 1, f);
            fclose(f);

            renderer.loadForegroundTexture(rgb, depth, mask, i);

            stbi_image_free(rgb);
            delete[] depth;
            delete[] mask;
        }
    }

    {
        string path = "./data/";
        for (int i = 0; i < renderer.num_camera; ++i) {
            int nrChannels;
            unsigned char *rgb = stbi_load(("/data/GoPro/videos/teaRoom/sequence/video/" + std::to_string(i + 1) + "-" +
                                            std::to_string(1) + ".png").c_str(), &renderer.widths[i],
                                           &renderer.heights[i],
                                           &nrChannels, 0);
            renderer.loadForegroundTexture(rgb, 0, 0, i);


            int width, height;
            width = 1600;
            height = 896;
            FILE *f= fopen(("/data/GoPro/videos/teaRoom/sequence/depth/0001/000" + std::to_string(i + 1) + ".pfm").c_str(), "rb");
//            ifstream depth_pfm("/data/GoPro/videos/teaRoom/sequence/depth/0001/000" + std::to_string(i + 1) + ".pfm");
//            depth_pfm >> tmp >> width >> height >> t1 >> t2;
            float *depth = new float[width * height];
            fread(depth, 22, 1, f);
            fread(depth, width * height * sizeof(float), 1, f);
            fclose(f);
//            int ret = depth_pfm.readsome(reinterpret_cast<char *>(depth), width * height * sizeof(float));
//            cout << ret << endl;
//            depth_pfm.close();

            renderer.loadForegroundTexture(0, depth, 0, i, width, height);

            unsigned char *mask;
            mask = stbi_load(
                    ("/data/GoPro/videos/teaRoom/sequence/mask/" + std::to_string(i + 1) + "-" + std::to_string(1) +
                     ".png").c_str(), &renderer.widths[i],
                    &renderer.heights[i],
                    &nrChannels, 0);

            renderer.loadForegroundTexture(0, 0, mask, i);

            stbi_image_free(rgb);
            delete[] depth;
            delete[] mask;
        }
    }
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        glPointSize(pointSize);
        glfwSetInputMode(window, GLFW_CURSOR, cursor_type);


        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT,
                                                0.1f,
                                                100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        renderer.setView(projection, view);
        renderer.clearBuffer();
//        renderer.renderBackground(show_type);
        renderer.renderForegroundFile(show_type, cam_select);
        renderer.renderBuffer();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float speed = 0.5f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        camera.Position = glm::vec3(0.0f, 0.0f, 0.0f);

//    cout << camera.Position[0] << " " << camera.Position[1] << " " << camera.Position[2] << endl;

    static bool flag = false;
    if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS && !flag)
        if (show_type == GL_FILL)
            show_type = GL_LINE;
        else if (show_type == GL_LINE)
            show_type = GL_POINT;
        else if (show_type == GL_POINT)
            show_type = GL_FILL;
    if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS)
        flag = true;
    else
        flag = false;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset * 0.2f, yoffset * 0.2f);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

