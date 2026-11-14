#pragma once

#include <Parameters/Globals.h>
#include <algorithm>

struct Mathx
{
    static Vector3D Lerp(const Vector3D& origin, const Vector3D& destination, float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);

        return origin + (destination - origin) * t;
    }

    static float Lerp(float origin, float destination, float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);

        return origin + (destination - origin) * t;
    }

    static inline float ToRadians(float degrees)
    {
        constexpr float PI = 3.14159265358979323846f;
        return degrees * PI / 180.0f;
    }
};
