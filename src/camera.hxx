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

#ifndef CAMERA_HXX
#define CAMERA_HXX

#include "common.hxx"

class Camera {
public:
    virtual bool sample_ray(Ray &ray, const glm::vec2 &uv) = 0;
};

class EquirectangularCamera final : public Camera {
public:
    EquirectangularCamera(float eye_altitude) : _eye_altitude(eye_altitude) {}
    virtual bool sample_ray(Ray &ray, const glm::vec2 &uv);
private:
    float _eye_altitude;
};

class FisheyeCamera final : public Camera {
public:
    FisheyeCamera(float eye_altitude, float aspect_ratio) :
        _eye_altitude(eye_altitude),
        _aspect_ratio(aspect_ratio) {}
    virtual bool sample_ray(Ray &ray, const glm::vec2 &uv);
private:
    float _eye_altitude;
    float _aspect_ratio;
};

#endif // CAMERA_HXX
