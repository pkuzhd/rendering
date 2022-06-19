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
              YAW, PITCH);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

#define N 640
#define M 384

float w = 384.0f;
float h = 640.0f;

int pointSize = 1;
int cam_select[5] = {0, 0, 1, 0, 0};
glm::vec3 centers[5] = {
        {0.        ,  0.        ,  0.        },
        {0.13558015,  0.09941017, -0.01442211},
        {0.27635732,  0.1841515 , -0.03728148},
        {0.40315295,  0.09187722, -0.0503269 },
        {0.53194052, -0.01007147, -0.05544123}
};

float centers_f[5][3] = {
        {0.        ,  0.        ,  0.        },
        {0.13558015,  0.09941017, -0.01442211},
        {0.27635732,  0.1841515 , -0.03728148},
        {0.40315295,  0.09187722, -0.0503269 },
        {0.53194052, -0.01007147, -0.05544123}
};

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

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("./shaders/camera.vert",
                     "./shaders/camera.frag");

    // set up vertex data (and buffer(s)) and configure vertqex attributes
    // ------------------------------------------------------------------
    float *vertices = new float[5 * (N + 1) * (M + 1)];
    for (int i = 0; i <= N; ++i) {
        for (int j = 0; j <= M; ++j) {
            vertices[(i * (M + 1) + j) * 5 + 0] = 1.0 / M * j;
            vertices[(i * (M + 1) + j) * 5 + 1] = 1.0 / N * i;
            vertices[(i * (M + 1) + j) * 5 + 2] = 0.0f;

            vertices[(i * (M + 1) + j) * 5 + 3] = 1.0 / M * j;
            vertices[(i * (M + 1) + j) * 5 + 4] = 1.0 / N * i;
        }
    }

    unsigned int *indices = new unsigned int[3 * N * M * 2];

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            indices[(i * M + j) * 6 + 0] = i * (M + 1) + j;
            indices[(i * M + j) * 6 + 1] = i * (M + 1) + j + 1;
            indices[(i * M + j) * 6 + 2] = (i + 1) * (M + 1) + j + 1;
            indices[(i * M + j) * 6 + 3] = i * (M + 1) + j;
            indices[(i * M + j) * 6 + 4] = (i + 1) * (M + 1) + j;
            indices[(i * M + j) * 6 + 5] = (i + 1) * (M + 1) + j + 1;
        }
    }

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 5 * (M + 1) * (N + 1), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * M * N * 3 * 2, indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    int width, height, nrChannels;
    unsigned char *data;
    width = 384;
    height = 640;

    unsigned int rgb_texture[5], depth_texture[5];
    glm::mat4 K_inv_list[5];
    glm::mat4 R_inv_list[5];

    std::string path = "./data/";

    for (int i = 0; i < 5; ++i) {
        glGenTextures(1, &rgb_texture[i]);
        glBindTexture(GL_TEXTURE_2D, rgb_texture[i]);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        unsigned char *data = stbi_load((path + std::to_string(i + 1) + ".png").c_str(), &width,
                                        &height,
                                        &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);

        glGenTextures(1, &depth_texture[i]);
        glBindTexture(GL_TEXTURE_2D, depth_texture[i]);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        data = reinterpret_cast<unsigned char *>(new float[640 * 384]);
        width = 384;
        height = 640;
        FILE *f = fopen((path + std::to_string(i + 1) + ".depth2").c_str(), "rb");
        fread(data, 640 * 384 * sizeof(float), 1, f);
        fclose(f);

//        memset(data, 0, 640 * 384 * sizeof(float));

//    for (int i = 160; i < 320; ++i) {
//        for (int j = 64; j < 192; ++j) {
//            ((float *) data)[i * 384 + j] = ((j - 64) / 128.0 / 2.0 + (i - 160) / 160.0 / 2.0)/10.0;
//        }
//    }
        // load image, create texture and generate mipmaps
//    data = stbi_load(std::string("resources/textures/awesomeface.png").c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RED, GL_FLOAT, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "Failed to load texture" << std::endl;
        }
//    stbi_image_free(data);
        delete[] data;

        float *para = new float[16];
        f = fopen((path + std::to_string(i + 1) + ".intr").c_str(), "rb");
        fread(para, 4 * sizeof(float), 1, f);
        fclose(f);

        K_inv_list[i] = glm::mat4(1);
        K_inv_list[i][0][0] = 1 / para[0];
        K_inv_list[i][1][1] = 1 / para[1];
        K_inv_list[i][0][2] = -para[2] / para[0];
        K_inv_list[i][1][2] = -para[3] / para[1];


        f = fopen((path + std::to_string(i + 1) + ".extr").c_str(), "rb");
        fread(para, 16 * sizeof(float), 1, f);
        fclose(f);

        R_inv_list[i] = glm::mat4(1);

        for (int j = 0; j < 16; ++j)
            R_inv_list[i][j / 4][j % 4] = para[j];

        cout << K_inv_list[i][0][0] << " " << K_inv_list[i][0][1] << " " << K_inv_list[i][0][2] << " " << K_inv_list[i][0][3] << endl;
        cout << K_inv_list[i][1][0] << " " << K_inv_list[i][1][1] << " " << K_inv_list[i][1][2] << " " << K_inv_list[i][1][3] << endl;
        cout << K_inv_list[i][2][0] << " " << K_inv_list[i][2][1] << " " << K_inv_list[i][2][2] << " " << K_inv_list[i][2][3] << endl;
        cout << K_inv_list[i][3][0] << " " << K_inv_list[i][3][1] << " " << K_inv_list[i][3][2] << " " << K_inv_list[i][3][3] << endl;
        cout << R_inv_list[i][0][0] << " " << R_inv_list[i][0][1] << " " << R_inv_list[i][0][2] << " " << R_inv_list[i][0][3] << endl;
        cout << R_inv_list[i][1][0] << " " << R_inv_list[i][1][1] << " " << R_inv_list[i][1][2] << " " << R_inv_list[i][1][3] << endl;
        cout << R_inv_list[i][2][0] << " " << R_inv_list[i][2][1] << " " << R_inv_list[i][2][2] << " " << R_inv_list[i][2][3] << endl;
        cout << R_inv_list[i][3][0] << " " << R_inv_list[i][3][1] << " " << R_inv_list[i][3][2] << " " << R_inv_list[i][3][3] << endl;
        cout << endl;

        delete[] para;
    }

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);


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

        glPolygonMode(GL_FRONT_AND_BACK, show_type);
        glPointSize(pointSize);
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        ourShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT,
                                                0.1f,
                                                100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        for (int i = 0; i < 5; ++i) {
            if (!cam_select[i])
                continue;
            // bind textures on corresponding texture units
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, rgb_texture[i]);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depth_texture[i]);

            glm::mat4 K_inv = K_inv_list[i];
            glm::mat4 R_inv = R_inv_list[i];
            ourShader.setMat4("K_inv", glm::transpose(K_inv));
            ourShader.setMat4("R_inv", glm::transpose(R_inv));
            ourShader.setFloat("width", width);
            ourShader.setFloat("height", height);

            // render boxes
            glBindVertexArray(VAO);
//        for (unsigned int i = 0; i < 10; i++) {
//            // calculate the model matrix for each object and pass it to shader before drawing
//            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
//            model = glm::translate(model, cubePositions[i]);
//            float angle = 20.0f * i;
//            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
//            ourShader.setMat4("model", model);
//
//            glDrawArrays(GL_TRIANGLES, 0, 36);
//        }

            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            float angle = 0.f;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
//        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 0.0f));
            ourShader.setMat4("model", model);
//            glDrawElements(GL_TRIANGLES, 6 * N * M, GL_UNSIGNED_INT, 0);
//            model = glm::translate(model, glm::vec3(-2.0f + i, 0.0f, 0.0f));
            ourShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 6 * N * M, GL_UNSIGNED_INT, 0);
        }

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

