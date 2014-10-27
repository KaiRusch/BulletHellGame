#include "vec2d.h"


//Returns the sum of two vectors
vec2d vecAdd(vec2d vectorA, vec2d vectorB)
{
    return vec2d(vectorA.x + vectorB.x, vectorA.y + vectorB.y);
}

//Returns the difference of two vectors
vec2d vecSub(vec2d vectorA, vec2d vectorB)
{
    return vec2d(vectorA.x - vectorB.x, vectorA.y - vectorB.y);
}

//Returns the product of a vector and a float
vec2d vecMult(vec2d vectorA, float B)
{
    return vec2d(vectorA.x*B,vectorA.y*B);
}

//Returns the quotient of a vector and a float
vec2d vecDiv(vec2d vectorA, float B)
{
    return vec2d(vectorA.x/B,vectorA.y/B);
}

//Returns a vector rotated by the angle theta
vec2d vecRot(vec2d vectorA, float theta)
{
    return vec2d(vectorA.x*cosf(theta)-vectorA.y*sinf(theta),vectorA.x*sinf(theta)+vectorA.y*cosf(theta));
}

//Returns a float of the dot product
float vecDot(vec2d vectorA, vec2d vectorB)
{
    return vectorA.x*vectorB.x+vectorA.x*vectorA.y;
}

//Returns the cross product of two vectors
float vecCross(vec2d vectorA, vec2d vectorB)
{
    return vectorA.x * vectorB.y - vectorA.y * vectorB.x;
}

//Returns magnitude squared of the vector
float vecMagSqr(vec2d vectorA)
{
    return vectorA.x*vectorA.x+vectorA.y*vectorA.y;
}


