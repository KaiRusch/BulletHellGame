#ifndef VEC2D_H
#define VEC2D_H

#include <cmath>

class vec2d
{
    public:
        float x;
        float y;
        vec2d() {x = 0.0f; y = 0.0f;};
        vec2d(float X, float Y) {x = X; y = Y;};
        float length(){return sqrtf(x*x+y*y);};
};

inline vec2d operator+(vec2d a, vec2d b)
{
    return vec2d(a.x+b.x,a.y+b.y);
}

inline void operator+=(vec2d &a, vec2d b)
{
    a.x = a.x + b.x;
    a.y = a.y + b.y;
}

inline vec2d operator-(vec2d a, vec2d b)
{
    return vec2d(a.x-b.x,a.y-b.y);
}

inline vec2d operator-(vec2d b)
{
    return vec2d(-b.x,-b.y);
}
inline void operator-=(vec2d &a, vec2d b)
{
    a.x = a.x - b.x;
    a.y = a.y - b.y;
}

inline vec2d operator*(vec2d a, float b)
{
    return vec2d(a.x*b,a.y*b);
}

inline vec2d operator*(float a, vec2d b)
{
    return vec2d(a*b.x,a*b.y);
}

inline void operator*=(vec2d &a, float b)
{
    a.x = a.x*b;
    a.y = a.y*b;
}

inline vec2d operator/(vec2d a, float b)
{
    return vec2d(a.x/b,a.y/b);
}

inline void operator/=(vec2d &a, float b)
{
    a.x = a.x/b;
    a.y = a.y/b;
}

vec2d vecRot(vec2d vectorA, float theta);
float vecDot(vec2d vectorA, vec2d vectorB);
float vecCross(vec2d vectorA, vec2d vectorB);
float vecMagSqr(vec2d vectorA);

#endif // VEC2D_H
