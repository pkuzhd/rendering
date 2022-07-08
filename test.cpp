#include <assert.h>
#include <iostream>
#include <chrono>
#include <string>
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
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "ply/PlyReader.h"
#include "ply/plyUtils.h"
#include "utils/Renderer.h"

using namespace std;

int main() {
    clock_t start_all = clock();
    cv::Mat *r;
    for (int j = 1; j <= 50; ++j) {
        clock_t start = clock();

        double t[3] = {0.0, 0.0, 0.0};

        int width, height;
        string path = "./data/";
        for (int i = 0; i < 5; ++i) {
            int nrChannels;
            clock_t t1 = clock();

//            unsigned char *rgb = stbi_load(("./data/sequence/video/" + std::to_string(i + 1) + "-" +
//                                            std::to_string(j) + ".png").c_str(), &width,
//                                           &height,
//                                           &nrChannels, 0);
            r = new cv::Mat;
            *r = cv::imread(("./data/sequence/video/" + std::to_string(i + 1) + "-" +
                             std::to_string(j) + ".png").c_str());
            clock_t t2 = clock();

            char filename[256];
            sprintf(filename, "./data/sequence/depth/%04d/%04d.pfm", j, i + 1);
            width = 1600;
            height = 896;
            FILE *f = fopen(filename, "rb");
//            ifstream depth_pfm("./data//sequence/depth/0001/000" + std::to_string(i + 1) + ".pfm");
//            depth_pfm >> tmp >> width >> height >> t1 >> t2;
            float *depth = new float[width * height];
            fread(depth, 22, 1, f);
            fread(depth, width * height * sizeof(float), 1, f);
            fclose(f);
//            int ret = depth_pfm.readsome(reinterpret_cast<char *>(depth), width * height * sizeof(float));
//            cout << ret << endl;
//            depth_pfm.close();
            clock_t t3 = clock();

            unsigned char *mask;
            mask = stbi_load(
                    ("./data/sequence/mask/" + std::to_string(i + 1) + "-" + std::to_string(j) +
                     ".png").c_str(), &width,
                    &height,
                    &nrChannels, 0);

            clock_t t4 = clock();
            std::cout << double(t2 - t1) / CLOCKS_PER_SEC << " "
                      << double(t3 - t2) / CLOCKS_PER_SEC << " "
                      << double(t4 - t3) / CLOCKS_PER_SEC << " "
                      << std::endl;
            t[0] += t2 - t1;
            t[1] += t3 - t2;
            t[2] += t4 - t3;
//            stbi_image_free(rgb);
//            delete r;
//            delete[] depth;
//            delete[] mask;
        }
        clock_t end = clock();
        std::cout << t[0] / CLOCKS_PER_SEC << " "
                  << t[1] / CLOCKS_PER_SEC << " "
                  << t[2] / CLOCKS_PER_SEC << " "
                  << double(end - start) / CLOCKS_PER_SEC << " "
                  << std::endl;

    }

    clock_t end_all = clock();
    std::cout << double(end_all - start_all) / CLOCKS_PER_SEC << "s" << std::endl;

    return 0;
}

//0.146895 0.001775 0.037557
//0.148161 0.0008 0.03466
//0.148093 0.000852 0.032625
//0.143431 0.00073 0.032544
//0.142644 0.000647 0.035103
//0.729224 0.004804 0.172489 0.916845
//0.131845 0.00056 0.03389
//0.130946 0.000434 0.033902
//0.128926 0.000623 0.03238
//0.128711 0.000432 0.031979
//0.128407 0.000437 0.033377
//0.648835 0.002486 0.165528 0.824553
//0.132884 0.000593 0.034194
//0.132545 0.000608 0.033717
//0.131297 0.000647 0.031303
//0.131021 0.000633 0.03264
//0.131481 0.000575 0.034375
//0.659228 0.003056 0.166229 0.837464 