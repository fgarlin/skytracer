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

#ifndef AEROSOL_HXX
#define AEROSOL_HXX

#include <cmath>

class Aerosol {
public:
    Aerosol(float turbidity,
            float base_density,
            float background_density,
            float height_scale) :
        _turbidity(turbidity),
        _base_density(base_density),
        _background_divided_by_base_density(background_density / base_density),
        _height_scale(height_scale) {}
    virtual ~Aerosol() {}

    virtual float get_absorption(float height, float wl) const {
        return get_absorption_cross_section(wl) * get_density(height) * _turbidity * 1e-3;
    }
    virtual float get_scattering(float height, float wl) const {
        return get_scattering_cross_section(wl) * get_density(height) * _turbidity * 1e-3;
    }
    virtual float get_extinction(float height, float wl) const {
        return (get_absorption_cross_section(wl) + get_scattering_cross_section(wl))
            * get_density(height) * _turbidity * 1e-3;
    }
protected:
    virtual float get_absorption_cross_section(float wl) const = 0;
    virtual float get_scattering_cross_section(float wl) const = 0;

    virtual float get_density(float height) const {
        height *= 1e-3; // To km
        return _base_density * (expf(-height / _height_scale) +
                                _background_divided_by_base_density);
    }

    float _turbidity;
    float _base_density;
    float _background_divided_by_base_density;
    float _height_scale;
};

class BackgroundAerosol : public Aerosol {
public:
    BackgroundAerosol(float turbidity) : Aerosol(turbidity, 2.584e17f, 2e6f, 0.0f) {}
    virtual ~BackgroundAerosol() {}
protected:
    virtual float get_density(float height) const override;
    virtual float get_absorption_cross_section(float wl) const override;
    virtual float get_scattering_cross_section(float wl) const override;
};

class DesertDustAerosol : public Aerosol {
public:
    DesertDustAerosol(float turbidity) : Aerosol(turbidity, 1.8662e18f, 2e6f, 2.0f) {}
    virtual ~DesertDustAerosol() {}
protected:
    virtual float get_absorption_cross_section(float wl) const override;
    virtual float get_scattering_cross_section(float wl) const override;
};

class MaritimeCleanAerosol : public Aerosol {
public:
    MaritimeCleanAerosol(float turbidity) : Aerosol(turbidity, 2.0266e17f, 2e6f, 0.9f) {}
    virtual ~MaritimeCleanAerosol() {}
protected:
    virtual float get_absorption_cross_section(float wl) const override;
    virtual float get_scattering_cross_section(float wl) const override;
};

class MaritimeMineralAerosol : public Aerosol {
public:
    MaritimeMineralAerosol(float turbidity) : Aerosol(turbidity, 2.0266e17f, 2e6f, 2.0f) {}
    virtual ~MaritimeMineralAerosol() {}
protected:
    virtual float get_absorption_cross_section(float wl) const override;
    virtual float get_scattering_cross_section(float wl) const override;
};

class PolarAntarcticAerosol : public Aerosol {
public:
    PolarAntarcticAerosol(float turbidity) : Aerosol(turbidity, 2.3864e16f, 2e6f, 30.0f) {}
    virtual ~PolarAntarcticAerosol() {}
protected:
    virtual float get_absorption_cross_section(float wl) const override;
    virtual float get_scattering_cross_section(float wl) const override;
};

class PolarArticAerosol : public Aerosol {
public:
    PolarArticAerosol(float turbidity) : Aerosol(turbidity, 2.3864e16f, 2e6f, 30.0f) {}
    virtual ~PolarArticAerosol() {}
protected:
    virtual float get_absorption_cross_section(float wl) const override;
    virtual float get_scattering_cross_section(float wl) const override;
};

class RemoteContinentalAerosol : public Aerosol {
public:
    RemoteContinentalAerosol(float turbidity) : Aerosol(turbidity, 6.103e18f, 2e6f, 0.73f) {}
    virtual ~RemoteContinentalAerosol() {}
protected:
    virtual float get_absorption_cross_section(float wl) const override;
    virtual float get_scattering_cross_section(float wl) const override;
};

class RuralAerosol : public Aerosol {
public:
    RuralAerosol(float turbidity) : Aerosol(turbidity, 8.544e18, 2e6f, 0.73f) {}
    virtual ~RuralAerosol() {}
protected:
    virtual float get_absorption_cross_section(float wl) const override;
    virtual float get_scattering_cross_section(float wl) const override;
};

class UrbanAerosol : public Aerosol {
public:
    UrbanAerosol(float turbidity) : Aerosol(turbidity, 1.3681e20f, 2e6f, 0.73f) {}
    virtual ~UrbanAerosol() {}
protected:
    virtual float get_absorption_cross_section(float wl) const override;
    virtual float get_scattering_cross_section(float wl) const override;
};

#endif // AEROSOL_HXX
