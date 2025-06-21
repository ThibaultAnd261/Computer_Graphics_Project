#include "mat4.hpp"

mat4::mat4() {
    data = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

mat4 mat4::operator*(const mat4& o) const {
    mat4 result;
    for (int row = 0; row < 4; ++row)
        for (int col = 0; col < 4; ++col) {
            result.data[col + row * 4] = 0.0f;
            for (int k = 0; k < 4; ++k)
                result.data[col + row * 4] += data[k + row * 4] * o.data[col + k * 4];
        }
    return result;
}
