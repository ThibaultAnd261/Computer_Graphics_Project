#pragma once
#include <array>

class mat4 {
public:
    std::array<float, 16> data;

    mat4();
    mat4 operator*(const mat4& other) const;
};
