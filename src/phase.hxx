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

#ifndef PHASE_HXX
#define PHASE_HXX

#include "common.hxx"

class PhaseFunction {
public:
    virtual float p(const glm::vec3 &wo, const glm::vec3 &wi, float wl) const = 0;
};

class Isotropic final : public PhaseFunction {
public:
    virtual float p(const glm::vec3 &wo, const glm::vec3 &wi, float wl) const {
        return M_INV_4PI;
    }
};

class HenyeyGreenstein final : public PhaseFunction {
public:
    HenyeyGreenstein(float g_) : g(g_) { gg = g*g; }
    virtual float p(const glm::vec3 &wo, const glm::vec3 &wi, float wl) const;
private:
    float g, gg;
};

class RayleighPhase final : public PhaseFunction {
public:
    virtual float p(const glm::vec3 &wo, const glm::vec3 &wi, float wl) const;
};

class ChandrasekharPhase final : public PhaseFunction {
public:
    virtual float p(const glm::vec3 &wo, const glm::vec3 &wi, float wl) const;
};

#endif // PHASE_HXX
