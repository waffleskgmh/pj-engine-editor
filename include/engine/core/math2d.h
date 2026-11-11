#pragma once

#include <cmath>
#include <string>
#include <cstdint>
#include <algorithm>

namespace PJEngine {

constexpr float PI = 3.14159265358979323846f;
constexpr float DEG2RAD = PI / 180.0f;
constexpr float RAD2DEG = 180.0f / PI;

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    Vec2() = default;
    Vec2(float x, float y) : x(x), y(y) {}

    // Operátory
    Vec2 operator+(const Vec2& o) const { return Vec2(x + o.x, y + o.y); }
    Vec2 operator-(const Vec2& o) const { return Vec2(x - o.x, y - o.y); }
    Vec2 operator*(float s) const { return Vec2(x * s, y * s); }
    Vec2 operator/(float s) const { return s != 0.0f ? Vec2(x / s, y / s) : Vec2(0.0f, 0.0f); }
    Vec2 operator-() const { return Vec2(-x, -y); }

    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }
    Vec2& operator*=(float s) { x *= s; y *= s; return *this; }
    Vec2& operator/=(float s) { if (s != 0.0f) { x /= s; y /= s; } return *this; }

    bool operator==(const Vec2& o) const { return std::abs(x - o.x) < 0.0001f && std::abs(y - o.y) < 0.0001f; }
    bool operator!=(const Vec2& o) const { return !(*this == o); }

    // Geometrické funkcie
    float lengthSq() const { return x * x + y * y; }
    float length() const { return std::sqrt(lengthSq()); }
    float dot(const Vec2& o) const { return x * o.x + y * o.y; }
    float cross(const Vec2& o) const { return x * o.y - y * o.x; } // 2D analóg vektorového súčinu

    Vec2 normalized() const {
        float l = length();
        return l > 0.00001f ? Vec2(x / l, y / l) : Vec2(0.0f, 0.0f);
    }

    float distanceTo(const Vec2& o) const { return (*this - o).length(); }
    float distanceSqTo(const Vec2& o) const { return (*this - o).lengthSq(); }

    Vec2 lerp(const Vec2& target, float t) const {
        return *this + (target - *this) * std::clamp(t, 0.0f, 1.0f);
    }

    Vec2 rotated(float radians) const {
        float c = std::cos(radians);
        float s = std::sin(radians);
        return Vec2(x * c - y * s, x * s + y * c);
    }

    static Vec2 Zero() { return Vec2(0.0f, 0.0f); }
    static Vec2 One() { return Vec2(1.0f, 1.0f); }
    static Vec2 Up() { return Vec2(0.0f, -1.0f); }
    static Vec2 Down() { return Vec2(0.0f, 1.0f); }
    static Vec2 Left() { return Vec2(-1.0f, 0.0f); }
    static Vec2 Right() { return Vec2(1.0f, 0.0f); }
};

struct Rect {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;

    Rect() = default;
    Rect(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}

    bool contains(const Vec2& p) const {
        return p.x >= x && p.x <= x + width && p.y >= y && p.y <= y + height;
    }

    bool intersects(const Rect& o) const {
        return !(x + width < o.x || o.x + o.width < x || y + height < o.y || o.y + o.height < y);
    }

    Vec2 center() const { return Vec2(x + width * 0.5f, y + height * 0.5f); }
    Vec2 size() const { return Vec2(width, height); }
    Vec2 position() const { return Vec2(x, y); }
};

struct Color {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;

    Color() = default;
    Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}

    uint32_t toRGBA32() const {
        uint8_t ur = static_cast<uint8_t>(std::clamp(r, 0.0f, 1.0f) * 255.0f);
        uint8_t ug = static_cast<uint8_t>(std::clamp(g, 0.0f, 1.0f) * 255.0f);
        uint8_t ub = static_cast<uint8_t>(std::clamp(b, 0.0f, 1.0f) * 255.0f);
        uint8_t ua = static_cast<uint8_t>(std::clamp(a, 0.0f, 1.0f) * 255.0f);
        return (ua << 24) | (ub << 16) | (ug << 8) | ur;
    }

    static Color White() { return Color(1.0f, 1.0f, 1.0f, 1.0f); }
    static Color Black() { return Color(0.0f, 0.0f, 0.0f, 1.0f); }
    static Color Red() { return Color(1.0f, 0.0f, 0.0f, 1.0f); }
    static Color Green() { return Color(0.0f, 1.0f, 0.0f, 1.0f); }
    static Color Blue() { return Color(0.0f, 0.0f, 1.0f, 1.0f); }
    static Color Yellow() { return Color(1.0f, 1.0f, 0.0f, 1.0f); }
    static Color Transparent() { return Color(0.0f, 0.0f, 0.0f, 0.0f); }
};

// 2D Afinná matica 3x2 pre preklad pozícií na GPU
struct Mat3x2 {
    float m[6] = {
        1.0f, 0.0f, // m0, m1 (Riadok 0)
        0.0f, 1.0f, // m2, m3 (Riadok 1)
        0.0f, 0.0f  // m4, m5 (Translácia X, Y)
    };

    static Mat3x2 Identity() {
        Mat3x2 r;
        return r;
    }

    static Mat3x2 Translation(const Vec2& pos) {
        Mat3x2 r;
        r.m[4] = pos.x;
        r.m[5] = pos.y;
        return r;
    }

    static Mat3x2 Rotation(float radians) {
        Mat3x2 r;
        float c = std::cos(radians);
        float s = std::sin(radians);
        r.m[0] = c;  r.m[1] = s;
        r.m[2] = -s; r.m[3] = c;
        return r;
    }

    static Mat3x2 Scale(const Vec2& scale) {
        Mat3x2 r;
        r.m[0] = scale.x;
        r.m[3] = scale.y;
        return r;
    }

    Mat3x2 multiply(const Mat3x2& o) const {
        Mat3x2 r;
        r.m[0] = m[0] * o.m[0] + m[1] * o.m[2];
        r.m[1] = m[0] * o.m[1] + m[1] * o.m[3];
        r.m[2] = m[2] * o.m[0] + m[3] * o.m[2];
        r.m[3] = m[2] * o.m[1] + m[3] * o.m[3];
        r.m[4] = m[4] * o.m[0] + m[5] * o.m[2] + o.m[4];
        r.m[5] = m[4] * o.m[1] + m[5] * o.m[3] + o.m[5];
        return r;
    }

    Vec2 transformPoint(const Vec2& p) const {
        return Vec2(
            p.x * m[0] + p.y * m[2] + m[4],
            p.x * m[1] + p.y * m[3] + m[5]
        );
    }
};

struct Transform2D {
    Vec2 position = Vec2(0.0f, 0.0f);
    float rotation = 0.0f; // Radiány
    Vec2 scale = Vec2(1.0f, 1.0f);
    Vec2 origin = Vec2(0.0f, 0.0f);

    Mat3x2 getMatrix() const {
        Mat3x2 t = Mat3x2::Translation(position);
        Mat3x2 r = Mat3x2::Rotation(rotation);
        Mat3x2 s = Mat3x2::Scale(scale);
        Mat3x2 o = Mat3x2::Translation(-origin);
        return o.multiply(s).multiply(r).multiply(t);
    }

    Vec2 transformPoint(const Vec2& p) const {
        return getMatrix().transformPoint(p);
    }
};

} // namespace PJEngine