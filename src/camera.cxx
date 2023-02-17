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

#include "camera.hxx"

using namespace glm;

//#define MORE_TEXELS_TO_HORIZON
//#define ONLY_RENDER_UPPER_HEMISPHERE

bool
EquirectangularCamera::sample_ray(Ray &ray, const glm::vec2 &uv)
{
    float phi = M_TWO_PI * uv.x;
#ifndef MORE_TEXELS_TO_HORIZON
#ifndef ONLY_RENDER_UPPER_HEMISPHERE
    float theta = M_PI * uv.y;
#else
    float theta = M_HALF_PI * uv.y;
#endif
#else
    // Apply a non-linear transformation to the elevation to dedicate more
    // texels to the horizon, which is where having more detail matters.
#ifndef ONLY_RENDER_UPPER_HEMISPHERE
    float l = uv.y * 2.0f - 1.0f;
#else
    float l = uv.y - 1.0f;
#endif
    float theta = l*l * sign(l) * M_PI * 0.5f + M_HALF_PI;
#endif
    ray.o = vec3(0.0f, 0.0f, _eye_altitude);
    ray.d = spherical_to_cartesian(theta, phi);
    return true;
}

bool
FisheyeCamera::sample_ray(Ray &ray, const glm::vec2 &uv_)
{
    glm::vec2 uv = uv_;
    uv -= 0.5f;
    if (_aspect_ratio < 1.0f) {
        uv.y *= 1.0f / _aspect_ratio;
    } else {
        uv.x *= _aspect_ratio;
    }
    float phi = atan2f(uv.y, uv.x);
    float l = length(uv);
    if (l > (0.5f + 1e-3f))
        return false;
    float theta = M_PI * l;
    ray.o = vec3(0.0f, 0.0f, _eye_altitude);
    ray.d = spherical_to_cartesian(theta, phi);
    return true;
}
