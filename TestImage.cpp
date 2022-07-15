#include <iostream>
#include "utils/ImageSender.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;

int main() {
    ImageSender sender;
    sender.open("./pipe_dir/pipe1");
    for (int j = 0; j < 5; ++j) {
        ImageData *data = new ImageData;
        data->n = 5;
        data->w = new int[data->n];
        data->h = new int[data->n];
        data->imgs = new char[data->n * 2160 * 3840 * 3];
        for (int i = 0; i < 5; ++i) {
            data->h[i] = 2160;
            data->w[i] = 3840;
            cv::Mat img = cv::imread("/data/GoPro/videos/teaRoom/sequence/video/" + to_string(i + 1) + "-" +
                                     to_string(j * 5 + 101) + ".png");
            memcpy(data->imgs + 2160 * 3840 * 3 * i, img.data, 2160 * 3840 * 3);
        }
        sender.sendData(data);
    }

    sender.close();
    return 0;
}
