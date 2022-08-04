//
// Created by zhd on 2022/8/4.
//

#ifndef RENDERING_FLAGS_H
#define RENDERING_FLAGS_H
#include <gflags/gflags.h>

DEFINE_string(f, "pipe", "input format");
DEFINE_string(input, "", "input name");
DEFINE_string(output, "", "output name");
DEFINE_string(cam, "", "camera parameter (json)");
DEFINE_string(mesh, "", "mesh filename (ply)");
DEFINE_string(texture, "", "texture filename (png,jpg,...)");

static bool validate_f(const char *flag, const string &value) {
    if (value != "file" && value != "pipe") {
        cout << "invalid value for " << flag << ": " << value << endl;
        return false;
    }
    return true;
}

static const bool f_error = gflags::RegisterFlagValidator(&fLS::FLAGS_f, validate_f);

#endif //RENDERING_FLAGS_H
