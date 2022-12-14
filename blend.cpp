#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <iostream>
#include <string>

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
              YAW - 12, PITCH + 2);

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

#define N 60
#define M 38

float w = 384.0f;
float h = 640.0f;

int cam_select[5] = {1, 1, 1, 1, 1};

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

int pointSize = 1;

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

    }
}

GLenum show_type = GL_FILL;

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

    GLint FBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &FBO);

    GLuint accumulateFBO;
    GLuint accumulateTexture;
    {
        glGenFramebuffers(1, &accumulateFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, accumulateFBO);

        glGenTextures(1, &accumulateTexture);
        glBindTexture(GL_TEXTURE_2D, accumulateTexture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0, // level
                GL_RGBA32F,
                SCR_WIDTH,
                SCR_HEIGHT,
                0, // border must be 0
                GL_RGBA,
                GL_BYTE,
                NULL); // no pixel data
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumulateTexture, 0);
    }

    GLuint cameraFBO;
    GLuint cameraTexture;
    GLuint cameraDepth;
    {
        glGenFramebuffers(1, &cameraFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, cameraFBO);

        glGenTextures(1, &cameraTexture);
        glBindTexture(GL_TEXTURE_2D, cameraTexture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0, // level
                GL_RGBA32F,
                SCR_WIDTH,
                SCR_HEIGHT,
                0, // border must be 0
                GL_RGBA,
                GL_BYTE,
                NULL); // no pixel data
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cameraTexture, 0);

        glGenRenderbuffers(1, &cameraDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, cameraDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, cameraDepth);
    }

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("./shaders/blend.vert",
                     "./shaders/blend.frag");
    Shader updateProgram("./shaders/update.vert",
                         "./shaders/update.frag");
    Shader resolveProgram("./shaders/resolve.vert",
                          "./shaders/resolve.frag");
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    int num_tri = 857132 * 2;
    float *vertices = new float[num_tri * 3 * 6];
    FILE *f = fopen("./data/xyzrgb", "rb");
    fread(vertices, num_tri * 3 * 6 * sizeof(float), 1, f);
    fclose(f);
//    int num_tri = 12;
//    float vertices[] = {
//            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
//            0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
//            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
//            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
//            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
//            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
//
//            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
//            0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
//            0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
//            0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
//            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
//            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
//
//            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
//            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
//            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
//            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
//            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
//            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
//
//            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
//            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
//            0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
//            0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
//            0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
//            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
//
//            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
//            0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
//            0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
//            0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
//            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
//            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
//
//            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
//            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
//            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
//            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
//            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
//            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f
//    };
    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f, 3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f, 2.0f, -2.5f),
            glm::vec3(1.5f, 0.2f, -1.5f),
            glm::vec3(-1.3f, 1.0f, -1.5f)
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 3 * num_tri, vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);



    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();

    updateProgram.use();
    updateProgram.setInt("sampler", 0);

    GLuint vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    const float tex[3][2] = {
            {0, 0},
            {0, 2},
            {2, 0},
    };


    unsigned int screenVBO, screenVAO;
    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);

    glBindVertexArray(screenVAO);

    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, tex, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(0);

    // configure global opengl state
    // -----------------------------
//    glEnable(GL_DEPTH_TEST);
//    glDisable(GL_DEPTH_TEST);
//
//    glEnable(GL_BLEND);
//    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ONE);

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

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                                100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, cubePositions[0]);
        float angle = 0;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));


        int tris[] = {0, 176124, 348068, 520032, 688200, 857132};
        // activate shader
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setMat4("model", model);
        glUniform3fv(glGetUniformLocation(ourShader.ID, "centers"), 5, reinterpret_cast<const GLfloat *>(centers_f));

        // clearAccumulation
        glBindFramebuffer(GL_FRAMEBUFFER, accumulateFBO);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
//        glEnable(GL_FRAMEBUFFER_SRGB);

        for (int i = 0; i < 5; ++i) {
            // clearSubframe
            glBindFramebuffer(GL_FRAMEBUFFER, cameraFBO);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // renderSubframe
            ourShader.use();
            ourShader.setVec3("center", centers[i]);
            ourShader.setInt("idx", i);
            glPolygonMode(GL_FRONT_AND_BACK, show_type);
            glPointSize(pointSize);
            glBindVertexArray(VAO);
            glEnable(GL_DEPTH_TEST);
            if (cam_select[i])
                glDrawArrays(GL_TRIANGLES, 6 * tris[i], 6 * (tris[i + 1] - tris[i]));
            glDisable(GL_DEPTH_TEST);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            // updateAccumulation
            updateProgram.use();
            glBindFramebuffer(GL_FRAMEBUFFER, accumulateFBO);
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);

            glBindVertexArray(screenVAO);
            glBindTexture(GL_TEXTURE_2D, cameraTexture);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glDisable(GL_BLEND);
        }

        // resolveAccumulation
        resolveProgram.use();
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        glBindVertexArray(screenVAO);
        glBindTexture(GL_TEXTURE_2D, accumulateTexture);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

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

