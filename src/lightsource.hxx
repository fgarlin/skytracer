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

#ifndef LIGHTSOURCE_HXX
#define LIGHTSOURCE_HXX

#include "common.hxx"

class LightSource {
public:
    LightSource(float elevation, float azimuth);
    virtual float eval(float wl) const = 0;
    virtual float sample(const glm::vec2 &sample, glm::vec3 &wi, float wl) const = 0;
protected:
    glm::vec3 direction;
    glm::mat3 light_to_world;
};

class DirectionalLight : public LightSource {
public:
    DirectionalLight(float elevation, float azimuth);
    virtual float eval(float wl) const = 0;
    virtual float sample(const glm::vec2 &sample, glm::vec3 &wi, float wl) const final;
};

class DistantDisk : public LightSource {
public:
    DistantDisk(float elevation, float azimuth);
    virtual float eval(float wl) const = 0;
    virtual float sample(const glm::vec2 &sample, glm::vec3 &wi, float wl) const final;
};

class Sun : public DirectionalLight {
public:
    Sun(float elevation, float azimuth);
    virtual float eval(float wl) const final;
};

#endif // LIGHTSOURCE_HXX
