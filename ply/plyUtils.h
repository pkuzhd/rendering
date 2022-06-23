#ifndef PLYUTILS_H
#define PLYUTILS_H

#define my_value_t double
#define my_index_t int64_t
#define my_byte_t uint64_t
#define my_string_t std::string
#define elementVertex 0
#define elementFace 1
using namespace SOLUTION;
struct comment {
    std::string type;
    std::string address;
};

comment *getComment(PlyReader *reader) {
    comment *nComment = new comment;
    nComment->type = reader->get_comment_type();
    nComment->address = reader->get_comment_address();
    return nComment;
}

PlyReader *initialReader(std::string const &path) {
    return new PlyReader(path);
}

double *initialDoubleArray(PlyReader *reader, int elementIndex, int propertySize, int *elementSize) {
    *elementSize = reader->element_size(elementIndex);
    return new double[propertySize * (*elementSize)];
}

long *initialLongArray(PlyReader *reader, int elementIndex, int propertySize, int *elementSize) {
    *elementSize = reader->element_size(elementIndex);
    return new long[propertySize * (*elementSize)];
}

bool getArrayfromPly(PlyReader *reader, double *vertexArray, double *coordArray, long *indexArray) {
    assert(reader->element_num() == 2);
    for (int index = 0; index < reader->element_num(); index++) {
        auto elementSize = reader->element_size(index);
        for (int size = 0; size < elementSize; size++) {
            switch (index) {
                //0 = vertex property = 3
                case 0: {
                    for (int propertySize = 0; propertySize < 3; propertySize++) {
                        vertexArray[size * 3 + propertySize] = reader->get_value(index, size, propertySize);
                    }
                    break;
                }
                case 1: {
                    for (int propertySize = 0; propertySize < 3; propertySize++) {
                        indexArray[size * 3 + propertySize] = reader->get_byte_list(size, propertySize);
                    }
                    for (int propertySize = 0; propertySize < 6; propertySize++) {
                        coordArray[size * 6 + propertySize] = reader->get_value_list(size, propertySize);
                    }
                    break;
                }
                default:
                    break;

            }
        }
    }
    return true;
}

bool
deleteItems(PlyReader *reader, double *vertexArray, double *coordArray, long *indexArray, comment *textureComment) {
    delete reader;
    delete vertexArray;
    delete coordArray;
    delete indexArray;
    delete textureComment;
    return true;
}

#endif