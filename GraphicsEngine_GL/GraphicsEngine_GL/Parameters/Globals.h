#pragma once
#include <cmath>
#include <iostream>
#include <glad/glad.h>

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

struct Vector2D {
    float x;
    float y;

    Vector2D(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }

    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }

    Vector2D operator*(float scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }

    Vector2D operator/(float scalar) const {
        if (scalar == 0) {
            throw std::runtime_error("Divided by zero!");
        }
        return Vector2D(x / scalar, y / scalar);
    }

    float dot(const Vector2D& other) const {
        return x * other.x + y * other.y;
    }

    float magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    Vector2D normalized() const {
        float mag = magnitude();
        if (mag == 0) {
            throw std::runtime_error("Cannot normalize a vector by 0!");
        }
        return *this / mag;
    }

    void setAsUniform(GLint location) const {
        glUniform2f(location, x, y);
    }

    void setAsVertexAttrib(GLuint index) const {
        glVertexAttrib2f(index, x, y);
    }

    bool operator==(const Vector2D& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Vector2D& other) const {
        return !(*this == other);
    }

    void print() const {
        std::cout << "(" << x << ", " << y << ")" << std::endl;
    }
};

struct Vector3D {
    float x;
    float y;
    float z;

    Vector3D(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

    Vector3D operator+(const Vector3D& other) const {
        return Vector3D(x + other.x, y + other.y, z + other.z);
    }

    Vector3D operator-(const Vector3D& other) const {
        return Vector3D(x - other.x, y - other.y, z - other.z);
    }

    Vector3D operator*(float scalar) const {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }

    Vector3D operator/(float scalar) const {
        if (scalar == 0) {
            throw std::runtime_error("Divided by zero!");
        }
        return Vector3D(x / scalar, y / scalar, z / scalar);
    }

    float dot(const Vector3D& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3D cross(const Vector3D& other) const {
        return Vector3D(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    float magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vector3D normalized() const {
        float mag = magnitude();
        if (mag == 0) {
            throw std::runtime_error("Cannot normalize a vector by 0!");
        }
        return *this / mag;
    }

    void setAsUniform(GLint location) const {
        glUniform3f(location, x, y, z);
    }

    void setAsVertexAttrib(GLuint index) const {
        glVertexAttrib3f(index, x, y, z);
    }

    bool operator==(const Vector3D& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const Vector3D& other) const {
        return !(*this == other);
    }

    void print() const {
        std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
    }
};

struct Quaternion {
    float w, x, y, z;

    Quaternion(float w = 1.0f, float x = 0.0f, float y = 0.0f, float z = 0.0f)
        : w(w), x(x), y(y), z(z) {}

    static Quaternion FromAxisAngle(const Vector3D& axis, float angle) {
        float halfAngle = angle * 0.5f;
        float s = std::sin(halfAngle);
        return Quaternion(std::cos(halfAngle), axis.x * s, axis.y * s, axis.z * s);
    }

    Quaternion operator*(const Quaternion& other) const {
        return Quaternion(
            w * other.w - x * other.x - y * other.y - z * other.z,
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w
        );
    }

    Quaternion conjugate() const {
        return Quaternion(w, -x, -y, -z);
    }

    Vector3D rotate(const Vector3D& v) const {
        Quaternion qv(0, v.x, v.y, v.z);
        Quaternion res = (*this) * qv * conjugate();
        return Vector3D(res.x, res.y, res.z);
    }
};

struct Mat4x4 {
    float m[4][4];

    Mat4x4() {
        memset(m, 0, sizeof(m));
        for (int i = 0; i < 4; ++i) {
            m[i][i] = 1.0f;
        }
    }

    Mat4x4(float values[4][4]) {
        memcpy(m, values, sizeof(m));
    }

    Mat4x4 operator*(const Mat4x4& other) const {
        Mat4x4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

    float* operator*(const float vec[4]) const {
        static float result[4];
        for (int i = 0; i < 4; ++i) {
            result[i] = 0.0f;
            for (int j = 0; j < 4; ++j) {
                result[i] += m[i][j] * vec[j];
            }
        }
        return result;
    }

    static Mat4x4 Translate(const Vector3D& pos)
    {
        Mat4x4 mat;
        mat.m[3][0] = pos.x;
        mat.m[3][1] = pos.y;
        mat.m[3][2] = pos.z;
        return mat;
    }

    static Mat4x4 Scale(const Vector3D& s)
    {
        Mat4x4 mat;
        mat.m[0][0] = s.x;
        mat.m[1][1] = s.y;
        mat.m[2][2] = s.z;
        return mat;
    }

    float* operator[](int index) { return m[index]; }
    const float* operator[](int index) const { return m[index]; }


    Mat4x4 Transpose() const {
        Mat4x4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = m[j][i];
            }
        }
        return result;
    }

    Mat4x4 Inverse() const {
        Mat4x4 result;
        float inv[16], det;
        float mat[16] = {
            m[0][0], m[0][1], m[0][2], m[0][3],
            m[1][0], m[1][1], m[1][2], m[1][3],
            m[2][0], m[2][1], m[2][2], m[2][3],
            m[3][0], m[3][1], m[3][2], m[3][3]
        };

        inv[0] = mat[5] * mat[10] * mat[15] - mat[5] * mat[11] * mat[14] -
            mat[9] * mat[6] * mat[15] + mat[9] * mat[7] * mat[14] +
            mat[13] * mat[6] * mat[11] - mat[13] * mat[7] * mat[10];

        det = mat[0] * inv[0];

        if (det == 0) {
            std::cerr << "[Mat4x4] WARNING: Attempted to invert a singular matrix!" << std::endl;
            return Mat4x4();
        }

        det = 1.0f / det;

        for (int i = 0; i < 16; i++)
            inv[i] *= det;

        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                result.m[i][j] = inv[i * 4 + j];

        return result;
    }

};

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_SIZE 1.0f