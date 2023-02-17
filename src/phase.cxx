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

#include "phase.hxx"

#include "lut.hxx"

using namespace glm;

namespace {
const float RAYLEIGH_PHASE_SCALE = (3.0f / 16.0f) * M_INV_PI;

/**
 * Tabulated values for the gamma term in the Chandrasekhar phase function.
 * A. Bucholtz 1995. Rayleigh-scattering calculations for the terrestrial atmosphere.
 * http://augerlal.lal.in2p3.fr/pmwiki/uploads/Bucholtz.pdf
 */
static const LookupTable gamma_lut = {
    {200.0f, 0.02326f},
    {205.0f, 0.02241f},
    {210.0f, 0.02100f},
    {215.0f, 0.02043f},
    {220.0f, 0.01986f},
    {225.0f, 0.01930f},
    {240.0f, 0.01872f},
    {260.0f, 0.01758f},
    {270.0f, 0.01729f},
    {280.0f, 0.01672f},
    {290.0f, 0.01643f},
    {300.0f, 0.01614f},
    {310.0f, 0.01614f},
    {320.0f, 0.01586f},
    {330.0f, 0.01557f},
    {340.0f, 0.01557f},
    {350.0f, 0.01528f},
    {360.0f, 0.01528f},
    {370.0f, 0.01528f},
    {380.0f, 0.01499f},
    {390.0f, 0.01499f},
    {400.0f, 0.01499f},
    {450.0f, 0.01471f},
    {500.0f, 0.01442f},
    {550.0f, 0.01442f},
    {600.0f, 0.01413f},
    {650.0f, 0.01413f},
    {700.0f, 0.01413f},
    {750.0f, 0.01413f},
    {800.0f, 0.01384f},
    {850.0f, 0.01384f},
    {900.0f, 0.01384f},
    {950.0f, 0.01384f},
    {1000.0f, 0.01384f},
};
} // anonymous namespace

//------------------------------------------------------------------------------

float
HenyeyGreenstein::p(const vec3 &wo, const vec3 &wi, float wl) const
{
    float cos_theta = dot(wo, wi);
    float denom = 1.0f + gg + 2.0f * g * cos_theta;
    return M_INV_4PI * (1.0f - gg) / (denom * sqrtf(denom));
}

//------------------------------------------------------------------------------

float
RayleighPhase::p(const vec3 &wo, const vec3 &wi, float wl) const
{
    float cos_theta = dot(wo, wi);
    return RAYLEIGH_PHASE_SCALE * (1.0f + cos_theta*cos_theta);
}

//------------------------------------------------------------------------------

float
ChandrasekharPhase::p(const vec3 &wo, const vec3 &wi, float wl) const
{
    float cos_theta = dot(wo, wi);
    float gamma = lut_lerp(gamma_lut, wl);
    return (RAYLEIGH_PHASE_SCALE / (1.0f + 2.0f * gamma))
        * (1.0f + 3.0f * gamma + (1.0f - gamma) * cos_theta*cos_theta);
}
