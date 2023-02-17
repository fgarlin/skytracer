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

#ifndef ATMOSPHERE_HXX
#define ATMOSPHERE_HXX

#include <memory>

#include "aerosol.hxx"
#include "common.hxx"
#include "phase.hxx"

class Atmosphere {
public:
    virtual float phase_eval(const glm::vec3 &p, float sample,
                             const glm::vec3 &wo, const glm::vec3 &wi,
                             float wl) const = 0;

    virtual float get_absorption(float height, float wl) const = 0;
    virtual float get_scattering(float height, float wl) const = 0;
    virtual float get_extinction(float height, float wl) const = 0;

    virtual float get_max_extinction(float wl) const {
        // Cache the result
        static float prev_wl = 0.0f;
        static float prev_max_extinction = 0.0f;
        if (prev_wl == wl) {
            return prev_max_extinction;
        }

        // Assume that the maximum extinction is at ground level
        float max_extinction = get_extinction(0.0f, wl);
        prev_wl = wl;
        prev_max_extinction = max_extinction;
        return max_extinction;
    }

    virtual float get_scattering_albedo(float height, float wl) const {
        float scattering = get_scattering(height, wl);
        float extinction = get_absorption(height, wl) + scattering;
        return scattering / extinction;
    }

    // Helper methods to use 3D points instead of altitudes
    float get_absorption(const glm::vec3 &p, float wl) const {
        return get_absorption(height_at_point(p), wl);
    }
    float get_scattering(const glm::vec3 &p, float wl) const {
        return get_scattering(height_at_point(p), wl);
    }
    float get_extinction(const glm::vec3 &p, float wl) const {
        return get_extinction(height_at_point(p), wl);
    }
    float get_scattering_albedo(const glm::vec3 &p, float wl) const {
        return get_scattering_albedo(height_at_point(p), wl);
    }
protected:
    float height_at_point(const glm::vec3 &p) const {
        return glm::distance(p, EARTH_CENTER) - EARTH_RADIUS;
    }
};

class GuimeraAtmosphere final : public Atmosphere {
public:
    GuimeraAtmosphere(int month, float turbidity,
                      const std::string &aerosol_type);

    float phase_eval(const glm::vec3 &p, float sample,
                     const glm::vec3 &wo, const glm::vec3 &wi,
                     float wl) const override;

    float get_scattering(float height, float wl) const override;
    float get_absorption(float height, float wl) const override;
    float get_extinction(float height, float wl) const override;
private:
    float get_molecular_scattering(float height, float wl) const;
    float get_molecular_absorption(float height, float wl) const;
    float get_ozone_height_distribution(float height) const;

    int _month;
    std::unique_ptr<PhaseFunction> _phase_molecular;
    std::unique_ptr<PhaseFunction> _phase_aerosol;
    std::unique_ptr<Aerosol> _aerosol;
};

#endif // ATMOSPHERE_HXX
