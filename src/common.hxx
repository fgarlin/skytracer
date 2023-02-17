// Copyright (C) 2022  Fernando García Liñán
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef COMMON_HXX
#define COMMON_HXX

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

const float M_TWO_PI  = 6.28318530717958647692;
const float M_HALF_PI = 1.57079632679489661923;
const float M_INV_PI  = 0.31830988618379067154;
const float M_INV_4PI = 0.07957747154594766788;

const glm::vec3 WORLD_UP(0.0f, 0.0f, 1.0f);

const float EARTH_RADIUS = 6371e3f;
const float ATMOSPHERE_THICKNESS = 1e5f;
const float ATMOSPHERE_RADIUS = EARTH_RADIUS + ATMOSPHERE_THICKNESS;
const glm::vec3 EARTH_CENTER(0.0f, 0.0f, -EARTH_RADIUS);

const float SUN_ANGULAR_DIAMETER = 0.00951204442f; // 0.545 degrees in radians
const float SUN_COS_THETA = 0.99998869014f; // cos(SUN_ANGULAR_DIAMETER / 2)

struct Ray {
    Ray() {}
    Ray(glm::vec3 origin, glm::vec3 direction) : o(origin), d(direction) {}

    glm::vec3 o;
    glm::vec3 d;
};

/**
 * Transform a set of spherical coordinates to Cartesian coordinates on the
 * unit sphere (r=1). The resulting vector is consequently normalized.
 * Theta is the inclination/polar angle and phi is the azimuth angle. Both are
 * expected to be in radians.
 */
glm::vec3 spherical_to_cartesian(float theta, float phi);

template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

#endif // COMMON_HXX
