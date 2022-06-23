#include <assert.h>
#include <iostream>
#include <chrono>


#include "ply/PlyReader.h"
#include "ply/plyUtils.h"

int main() {
    double *vertexArray, *coordArray;
    int vertexSize = 0;
    int faceIndexSize = 0;
    int faceCoordSize = 0;

    {
        clock_t start = clock();
        int vertexPropertySize = 3;
        int faceIndexPropertySize = 3;
        int faceCoordPorpertySize = 6;
        std::string path = "data/scene_dense_mesh_refine_texture.ply";
        PlyReader *reader = initialReader(path);
        double *tmp = initialDoubleArray(reader, elementVertex, vertexPropertySize, &vertexSize);
        long *indexArray = initialLongArray(reader, elementFace, faceIndexPropertySize, &faceIndexSize);
        coordArray = initialDoubleArray(reader, elementFace, faceCoordPorpertySize, &faceCoordSize);

        assert(faceIndexSize == faceCoordSize);
//        comment *textureComment = getComment(reader);
        getArrayfromPly(reader, tmp, coordArray, indexArray);
//        clock_t end = clock();
//        auto b = deleteItems(reader, vertexArray, coordArray, indexArray, textureComment);
//        std::cout << double(end - start) / CLOCKS_PER_SEC << "s" << std::endl;
        vertexArray = new double[faceIndexSize * faceIndexPropertySize * 3];
        for (int i = 0; i < faceIndexSize; ++i) {
            vertexArray[9 * i + 0] = tmp[3 * indexArray[3 * i + 0] + 0];
            vertexArray[9 * i + 1] = tmp[3 * indexArray[3 * i + 0] + 1];
            vertexArray[9 * i + 2] = tmp[3 * indexArray[3 * i + 0] + 2];
            vertexArray[9 * i + 3] = tmp[3 * indexArray[3 * i + 1] + 0];
            vertexArray[9 * i + 4] = tmp[3 * indexArray[3 * i + 1] + 1];
            vertexArray[9 * i + 5] = tmp[3 * indexArray[3 * i + 1] + 2];
            vertexArray[9 * i + 6] = tmp[3 * indexArray[3 * i + 2] + 0];
            vertexArray[9 * i + 7] = tmp[3 * indexArray[3 * i + 2] + 1];
            vertexArray[9 * i + 8] = tmp[3 * indexArray[3 * i + 2] + 2];
        }
        delete[]tmp;
        delete[]indexArray;
    }
    return 0;
}