#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "ply/PlyReader.h"
#include "ply/plyUtils.h"
#include "utils/Renderer.h"
#include "utils/ThreadPool.h"
#include "utils/RGBDReceiver.h"
#include "utils/FileRGBDReceiver.h"


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

#define N 960
#define M 540

float w = 384.0f;
float h = 640.0f;

int pointSize = 1;
int cam_select[5] = {1, 1, 1, 1, 1};
glm::vec3 centers[5] = {
        {0.,         0.,          0.},
        {0.13442005, -0.00617611, 0.00693867},
        {0.26451552, -0.01547954, 0.01613046},
        {0.39635817, -0.01958267, 0.02322004},
        {0.53632535, -0.02177305, 0.05059797}
};

GLuint show_type = GL_FILL;
GLuint cursor_type = GLFW_CURSOR_NORMAL;

bool pause = false;
bool keyleft = false, keyright = false;

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
        if (key == GLFW_KEY_SPACE)
            pause = !pause;
        if (key == GLFW_KEY_LEFT)
            keyleft = true;
        if (key == GLFW_KEY_RIGHT)
            keyright = true;
    }
    if (action == GLFW_REPEAT) {
        if (key == GLFW_KEY_LEFT)
            keyleft = true;
        if (key == GLFW_KEY_RIGHT)
            keyright = true;
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
    glfwSetInputMode(window, GLFW_CURSOR, cursor_type);

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
    renderer.loadBackground("/data/colmapTest/backgroundPly/bc5/scene_dense_mesh_refine_texture.ply",
                            "/data/colmapTest/backgroundPly/bc5/scene_dense_mesh_refine_texture.png");
    renderer.loadForegroundFile("./data/para.json", M, N);

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    renderer.foregroundProgram->use();
    renderer.foregroundProgram->setInt("rgb", 0);
    renderer.foregroundProgram->setInt("depth", 1);
    renderer.foregroundProgram->setInt("mask", 2);


    int num_thread = 32;
    ThreadPool threadPool(num_thread);

//    void *rgbs[100][5];
//    void *depths[100][5];
//    void *masks[100][5];

    int framerate = 10;
    int begin = 101;
    int step = 50 / framerate;
    int num_frame = 5;

//    IRGBDReceiver *receiver = new RGBDReceiver();
//    receiver->open("./pipe_dir/pipe2");
    IRGBDReceiver *receiver = new FileRGBDReceiver();
    RGBDData *data = nullptr;

    auto t1 = chrono::high_resolution_clock::now();
    receiver->open("/data/GoPro/videos/teaRoom/sequence/1080p/");
    int num = 1;
    for (int j = 0; j < num; ++j) {
        data = nullptr;
        while (!data) {
            data = receiver->getData();
        };
        for (int i = 0; i < renderer.num_camera; ++i) {
            renderer.widths[i] = data->w[i];
            renderer.heights[i] = data->h[i];
            renderer.w_crop[i] = data->w_crop[i];
            renderer.h_crop[i] = data->h_crop[i];
            renderer.x_crop[i] = data->x[i];
            renderer.y_crop[i] = data->y[i];
        }
    }
    auto t2 = chrono::high_resolution_clock::now();
    int size;
    size = receiver->getBufferSize();
    cout << chrono::duration<double, milli>(t2 - t1).count() / 1000 << " "
         << num / (chrono::duration<double, milli>(t2 - t1).count() / 1000)
         << " buffer: " << size << endl;

    for (int i = 0; i < 5; ++i) {
        renderer.loadForegroundTexture(data->getImage(i), 0, 0, i);
        renderer.loadForegroundTexture(0, data->getDepth(i), 0, i);
        renderer.loadForegroundTexture(0, 0, data->getMask(i), i);
    }

    delete data;
    data = nullptr;

    glm::mat4 model = {
            0.997040, -0.014666, -0.075476, 4.976910,
            -0.015422, -0.999836, -0.009449, -0.337819,
            -0.075325, 0.010585, -0.997103, 0.265862,
            0.000000, -0.000000, 0.000000, 1.000000
    };
    renderer.setModel(model);
//    renderer.setModel(glm::mat4(1.0f));

    // render loop
    // -----------
    float start_time = static_cast<float>(glfwGetTime());
    int last_id = 0;
    framerate = 25;
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        int next_frame = false;

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        int frame_id = (currentFrame - start_time) * framerate;
        if (frame_id > last_id) {
            last_id = frame_id;
            next_frame = true;
        }
        int size;
        size = receiver->getBufferSize();
//        cout << frame_id << " " << next_frame << " buffer: " << size << endl;
//        if (size < 100)
//            this_thread::sleep_for(chrono::milliseconds(500));


        static bool is_pause = false;
        if (pause) {
            if (!is_pause) {
                is_pause = true;
            }
            if (keyright) {
                keyright = false;
                data = nullptr;
                while (!data) {
                    data = receiver->getData();
                };
                for (int i = 0; i < renderer.num_camera; ++i) {
                    renderer.widths[i] = data->w[i];
                    renderer.heights[i] = data->h[i];
                    renderer.w_crop[i] = data->w_crop[i];
                    renderer.h_crop[i] = data->h_crop[i];
                    renderer.x_crop[i] = data->x[i];
                    renderer.y_crop[i] = data->y[i];
                }
                for (int i = 0; i < 5; ++i) {
                    renderer.loadForegroundTexture(data->getImage(i), 0, 0, i);
                    renderer.loadForegroundTexture(0, data->getDepth(i), 0, i);
                    renderer.loadForegroundTexture(0, 0, data->getMask(i), i);
                }
                delete data;
                data = nullptr;
            }
        } else {
            keyleft = false;
            keyright = false;
            if (is_pause) {
                is_pause = false;
            }
            if (next_frame) {
                data = nullptr;
                while (!data) {
                    data = receiver->getData();
                };
                for (int i = 0; i < renderer.num_camera; ++i) {
                    renderer.widths[i] = data->w[i];
                    renderer.heights[i] = data->h[i];
                    renderer.w_crop[i] = data->w_crop[i];
                    renderer.h_crop[i] = data->h_crop[i];
                    renderer.x_crop[i] = data->x[i];
                    renderer.y_crop[i] = data->y[i];
                }
                for (int i = 0; i < 5; ++i) {
                    renderer.loadForegroundTexture(data->getImage(i), 0, 0, i);
                    renderer.loadForegroundTexture(0, data->getDepth(i), 0, i);
                    renderer.loadForegroundTexture(0, 0, data->getMask(i), i);
                }
                delete data;
                data = nullptr;
            }
        }


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

