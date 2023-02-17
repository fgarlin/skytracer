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

#include "atmosphere.hxx"
#include "lut.hxx"

#include <array>
#include <iostream>

namespace {

// Temperature and pressure for standard air
static const float Ts = 288.15f;   // Kelvin
static const float Ps = 101325.0f; // Pa

/**
 * Rayleigh volume scattering coefficient beta_s (km^-1) for standard air
 * (Ps = 1013.25 mbars, Ts = 288.15K) and for wavelengths from 200nm to 4000nm.
 * A. Bucholtz 1995. Rayleigh-scattering calculations for the terrestrial atmosphere.
 * http://augerlal.lal.in2p3.fr/pmwiki/uploads/Bucholtz.pdf
 */
static const LookupTable rayleigh_volume_scattering_lut = {
    {200.0f, 9.202e-1f},
    {210.0f, 7.225e-1f},
    {220.0f, 5.781e-1f},
    {230.0f, 4.691e-1f},
    {240.0f, 3.859e-1f},
    {250.0f, 3.208e-1f},
    {260.0f, 2.690e-1f},
    {270.0f, 2.277e-1f},
    {280.0f, 1.940e-1f},
    {290.0f, 1.665e-1f},
    {300.0f, 1.437e-1f},
    {310.0f, 1.249e-1f},
    {320.0f, 1.090e-1f},
    {330.0f, 9.557e-2f},
    {340.0f, 8.425e-2f},
    {350.0f, 7.450e-2f},
    {360.0f, 6.619e-2f},
    {370.0f, 5.901e-2f},
    {380.0f, 5.275e-2f},
    {390.0f, 4.734e-2f},
    {400.0f, 4.261e-2f},
    {410.0f, 3.845e-2f},
    {420.0f, 3.479e-2f},
    {430.0f, 3.156e-2f},
    {440.0f, 2.870e-2f},
    {450.0f, 2.616e-2f},
    {460.0f, 2.389e-2f},
    {470.0f, 2.187e-2f},
    {480.0f, 2.005e-2f},
    {490.0f, 1.842e-2f},
    {500.0f, 1.696e-2f},
    {510.0f, 1.564e-2f},
    {520.0f, 1.444e-2f},
    {530.0f, 1.336e-2f},
    {540.0f, 1.238e-2f},
    {550.0f, 1.149e-2f},
    {560.0f, 1.067e-2f},
    {570.0f, 9.927e-3f},
    {580.0f, 9.247e-3f},
    {590.0f, 8.624e-3f},
    {600.0f, 8.053e-3f},
    {610.0f, 7.530e-3f},
    {620.0f, 7.049e-3f},
    {630.0f, 6.605e-3f},
    {640.0f, 6.196e-3f},
    {650.0f, 5.819e-3f},
    {660.0f, 5.470e-3f},
    {670.0f, 5.146e-3f},
    {680.0f, 4.847e-3f},
    {690.0f, 4.568e-3f},
    {700.0f, 4.310e-3f},
    {710.0f, 4.069e-3f},
    {720.0f, 3.846e-3f},
    {730.0f, 3.637e-3f},
    {740.0f, 3.442e-3f},
    {750.0f, 3.261e-3f},
    {760.0f, 3.090e-3f},
    {770.0f, 2.931e-3f},
    {780.0f, 2.781e-3f},
    {790.0f, 2.641e-3f},
    {800.0f, 2.510e-3f},
    {900.0f, 1.561e-3f},
    {1000.0f, 1.022e-3f},
    {1100.0f, 6.964e-4f},
    {1200.0f, 4.909e-4f},
    {1300.0f, 3.560e-4f},
    {1400.0f, 2.644e-4f},
    {1500.0f, 2.005e-4f},
    {1600.0f, 1.548e-4f},
    {1700.0f, 1.214e-4f},
    {1800.0f, 9.656e-5f},
    {1900.0f, 7.775e-5f},
    {2000.0f, 6.331e-5f},
    {2200.0f, 4.322e-5f},
    {2400.0f, 3.050e-5f},
    {2600.0f, 2.214e-5f},
    {2800.0f, 1.646e-5f},
    {3000.0f, 1.249e-5f},
    {3500.0f, 6.737e-6f},
    {4000.0f, 3.948e-6f},
};

/**
 * Temperature (K) as a function of height (km) from the US Standard Atmosphere.
 * US COESA 1976. Standard Atmosphere, 1976. US Government Printing Office.
 * http://ntrs.nasa.gov/archive/nasa/casi.ntrs.nasa.gov/19770009539.pdf
 */
static const LookupTable standard_atmosphere_temperature_lut = {
    {0.0f, 288.15f},
    {1.0f, 281.65f},
    {2.0f, 275.15f},
    {3.0f, 268.65f},
    {4.0f, 262.15f},
    {5.0f, 255.65f},
    {6.0f, 249.15f},
    {7.0f, 242.65f},
    {8.0f, 236.15f},
    {9.0f, 229.65f},
    {10.0f, 223.15f},
    {11.0f, 216.65f},
    {12.0f, 216.65f},
    {13.0f, 216.65f},
    {14.0f, 216.65f},
    {15.0f, 216.65f},
    {16.0f, 216.65f},
    {17.0f, 216.65f},
    {18.0f, 216.65f},
    {19.0f, 216.65f},
    {20.0f, 216.65f},
    {21.0f, 217.65f},
    {22.0f, 218.65f},
    {23.0f, 219.65f},
    {24.0f, 220.65f},
    {25.0f, 221.65f},
    {26.0f, 222.65f},
    {27.0f, 223.65f},
    {28.0f, 224.65f},
    {29.0f, 225.65f},
    {30.0f, 226.65f},
    {31.0f, 227.65f},
    {32.0f, 228.65f},
    {33.0f, 231.45f},
    {34.0f, 234.25f},
    {35.0f, 237.05f},
    {36.0f, 239.85f},
    {37.0f, 242.65f},
    {38.0f, 245.45f},
    {39.0f, 248.25f},
    {40.0f, 251.05f},
    {41.0f, 253.85f},
    {42.0f, 256.65f},
    {43.0f, 259.45f},
    {44.0f, 262.25f},
    {45.0f, 265.05f},
    {46.0f, 267.85f},
    {47.0f, 270.65f},
    {48.0f, 270.65f},
    {49.0f, 270.65f},
    {50.0f, 270.65f},
    {51.0f, 270.65f},
    {52.0f, 267.85f},
    {53.0f, 265.05f},
    {54.0f, 262.25f},
    {55.0f, 259.45f},
    {56.0f, 256.65f},
    {57.0f, 253.85f},
    {58.0f, 251.05f},
    {59.0f, 248.25f},
    {60.0f, 245.45f},
    {61.0f, 242.65f},
    {62.0f, 239.85f},
    {63.0f, 237.05f},
    {64.0f, 234.25f},
    {65.0f, 231.45f},
    {66.0f, 228.65f},
    {67.0f, 225.85f},
    {68.0f, 223.05f},
    {69.0f, 220.25f},
    {70.0f, 217.45f},
    {71.0f, 214.65f},
    {72.0f, 212.65f},
    {73.0f, 210.65f},
    {74.0f, 208.65f},
    {75.0f, 206.65f},
    {76.0f, 204.65f},
    {77.0f, 202.65f},
    {78.0f, 200.65f},
    {79.0f, 198.65f},
    {80.0f, 196.65f},
    {81.0f, 194.65f},
    {82.0f, 192.65f},
    {83.0f, 190.65f},
    {84.0f, 188.65f},
    {85.0f, 186.946f},
    {86.0f, 186.946f},
};

/**
 * Pressure (Pa) as a function of height (km) from the US Standard Atmosphere.
 * US COESA 1976. Standard Atmosphere, 1976. US Government Printing Office.
 * http://ntrs.nasa.gov/archive/nasa/casi.ntrs.nasa.gov/19770009539.pdf
 */
static const LookupTable standard_atmosphere_pressure_lut = {
    {0.0f, 101325.0f},
    {1.0f, 89874.6f},
    {2.0f, 79495.2f},
    {3.0f, 70108.5f},
    {4.0f, 61640.2f},
    {5.0f, 54019.9f},
    {6.0f, 47181.0f},
    {7.0f, 41060.7f},
    {8.0f, 35599.8f},
    {9.0f, 30742.5f},
    {10.0f, 26436.3f},
    {11.0f, 22632.1f},
    {12.0f, 19330.4f},
    {13.0f, 16510.4f},
    {14.0f, 14101.8f},
    {15.0f, 12044.6f},
    {16.0f, 10287.5f},
    {17.0f, 8786.68f},
    {18.0f, 7504.84f},
    {19.0f, 6410.01f},
    {20.0f, 5474.89f},
    {21.0f, 4677.89f},
    {22.0f, 3999.79f},
    {23.0f, 3422.43f},
    {24.0f, 2930.49f},
    {25.0f, 2511.02f},
    {26.0f, 2153.09f},
    {27.0f, 1847.46f},
    {28.0f, 1586.29f},
    {29.0f, 1362.96f},
    {30.0f, 1171.87f},
    {31.0f, 1008.23f},
    {32.0f, 868.019f},
    {33.0f, 748.228f},
    {34.0f, 646.122f},
    {35.0f, 558.924f},
    {36.0f, 484.317f},
    {37.0f, 420.367f},
    {38.0f, 365.455f},
    {39.0f, 318.220f},
    {40.0f, 277.522f},
    {41.0f, 242.395f},
    {42.0f, 212.030f},
    {43.0f, 185.738f},
    {44.0f, 162.937f},
    {45.0f, 143.135f},
    {46.0f, 125.910f},
    {47.0f, 110.906f},
    {48.0f, 97.7545f},
    {49.0f, 86.1623f},
    {50.0f, 75.9448f},
    {51.0f, 66.9389f},
    {52.0f, 58.9622f},
    {53.0f, 51.8668f},
    {54.0f, 45.5632f},
    {55.0f, 39.9700f},
    {56.0f, 35.0137f},
    {57.0f, 30.6274f},
    {58.0f, 26.7509f},
    {59.0f, 23.3296f},
    {60.0f, 20.3143f},
    {61.0f, 17.6606f},
    {62.0f, 15.3287f},
    {63.0f, 13.2826f},
    {64.0f, 11.4900f},
    {65.0f, 9.92203f},
    {66.0f, 8.55275f},
    {67.0f, 7.35895f},
    {68.0f, 6.31992f},
    {69.0f, 5.41717f},
    {70.0f, 4.63422f},
    {71.0f, 3.95642f},
    {72.0f, 3.37176f},
    {73.0f, 2.86917f},
    {74.0f, 2.43773f},
    {75.0f, 2.06792f},
    {76.0f, 1.75140f},
    {77.0f, 1.48092f},
    {78.0f, 1.25012f},
    {79.0f, 1.05351f},
    {80.0f, 0.88628f},
    {81.0f, 0.74428f},
    {82.0f, 0.623905f},
    {83.0f, 0.522037f},
    {84.0f, 0.435981f},
    {85.0f, 0.36342f},
    {86.0f, 0.302723f},
};

/**
 * Ozone absolute absorption cross-section in cm^2 at 295+-3K.
 * Gorshelev 2014. High spectral resolution ozone absorption cross-sections.
 */
static const LookupTable ozone_cross_section_lut = {
    {244.0f, 946e-20f},
    {248.0f, 1051e-20f},
    {253.0f, 1120e-20f},
    {257.0f, 1107e-20f},
    {289.0f, 151e-20f},
    {296.0f, 61.1e-20f},
    {302.0f, 29.6e-20f},
    {365.0f, 4.9e-23f},
    {405.0f, 1.46e-23f},
    {455.0f, 20.6e-23f},
    {543.0f, 3.08e-21f},
    {576.0f, 4.70e-21f},
    {594.0f, 4.63e-21f},
    {604.0f, 5.10e-21f},
    {611.0f, 4.54e-21f},
    {632.0f, 3.36e-21f},
    {748.0f, 4.38e-22f},
    {755.0f, 3.22e-22f},
    {760.0f, 2.77e-22f},
    {765.0f, 2.53e-22f},
    {770.0f, 2.49e-22f},
    {779.0f, 3.15e-22f},
    {802.0f, 1.45e-22f},
    {817.0f, 2.20e-22f},
    {853.0f, 1.46e-22f},
    {877.0f, 0.377e-22f},
    {889.0f, 0.510e-22f},
    {898.0f, 0.638e-22f},
    {933.0f, 0.162e-22f},
    {944.0f, 0.424e-22f},
    {991.0f, 0.407e-22f},
    {1046.0f, 0.0773e-22f},
};

/**
 * Monthly mean values of total ozone across 45 years in Arosa, Switzerland (47ºN),
 * measured in Dobson.
 * Dutsch 1973. The Ozone distribution in the atmosphere.
 */
static const std::array<float, 12> ozone_mean_monthly_dobson = {
    347, // January
    370, // February
    381, // March
    384, // April
    372, // May
    352, // June
    333, // July
    317, // August
    298, // September
    285, // October
    290, // November
    315, // December
};

} // anonymous namespace

GuimeraAtmosphere::GuimeraAtmosphere(int month, float turbidity,
                                     const std::string &aerosol_type) :
    _month(month)
{
    if (_month < 0 || _month > 11) {
        std::cerr << "Invalid month number " << _month << ". Using January.\n";
        _month = 0;
    }
    _phase_molecular = std::make_unique<ChandrasekharPhase>();
    _phase_aerosol = std::make_unique<HenyeyGreenstein>(0.8);

    if (aerosol_type == "none") {
        // Do nothing
    } else if (aerosol_type == "background") {
        _aerosol = std::make_unique<BackgroundAerosol>(turbidity);
    } else if (aerosol_type == "desert-dust") {
        _aerosol = std::make_unique<DesertDustAerosol>(turbidity);
    } else if (aerosol_type == "maritime-clean") {
        _aerosol = std::make_unique<MaritimeCleanAerosol>(turbidity);
    } else if (aerosol_type == "maritime-mineral") {
        _aerosol = std::make_unique<MaritimeMineralAerosol>(turbidity);
    } else if (aerosol_type == "polar-antarctic") {
        _aerosol = std::make_unique<PolarAntarcticAerosol>(turbidity);
    } else if (aerosol_type == "polar-artic") {
        _aerosol = std::make_unique<PolarArticAerosol>(turbidity);
    } else if (aerosol_type == "remote-continental") {
        _aerosol = std::make_unique<RemoteContinentalAerosol>(turbidity);
    } else if (aerosol_type == "rural") {
        _aerosol = std::make_unique<RuralAerosol>(turbidity);
    } else if (aerosol_type == "urban") {
        _aerosol = std::make_unique<UrbanAerosol>(turbidity);
    } else {
        std::cerr << "Unknown aerosol type '" << aerosol_type
                  << "'. Using no aerosols.\n";
    }
}

float
GuimeraAtmosphere::phase_eval(const glm::vec3 &p, float sample,
                              const glm::vec3 &wo, const glm::vec3 &wi,
                              float wl) const
{
    if (!_aerosol) {
        return _phase_molecular->p(wo, wi, wl);
    }

    float height = height_at_point(p);
    float molecular_scattering = get_molecular_scattering(height, wl);
    float aerosol_scattering = _aerosol->get_scattering(height, wl);
    float molecular_scattering_probability = molecular_scattering /
        (molecular_scattering + aerosol_scattering);

    if (sample < molecular_scattering_probability) {
        return _phase_molecular->p(wo, wi, wl);
    } else {
        return _phase_aerosol->p(wo, wi, wl);
    }
}

float
GuimeraAtmosphere::get_scattering(float height, float wl) const
{
    float scattering = get_molecular_scattering(height, wl);
    if (_aerosol) {
        scattering += _aerosol->get_scattering(height, wl);
    }
    return scattering;
}

float
GuimeraAtmosphere::get_absorption(float height, float wl) const
{
    float absorption = get_molecular_absorption(height, wl);
    if (_aerosol) {
        absorption += _aerosol->get_absorption(height, wl);
    }
    return absorption;
}

float
GuimeraAtmosphere::get_extinction(float height, float wl) const
{
    float extinction = get_molecular_scattering(height, wl) +
        get_molecular_absorption(height, wl);
    if (_aerosol) {
        extinction += _aerosol->get_extinction(height, wl);
    }
    return extinction;
}

float
GuimeraAtmosphere::get_molecular_scattering(float height, float wl) const
{
    height *= 1e-3f; // To km
    float beta_s = lut_lerp(rayleigh_volume_scattering_lut, wl); // km^-1
    float T = lut_lerp(standard_atmosphere_temperature_lut, height); // K
    float P = lut_lerp(standard_atmosphere_pressure_lut, height); // Pa
    float beta = beta_s * (P / Ps) * (Ts / T); // km^-1
    return beta * 1e-3f; // m^-1
}

float
GuimeraAtmosphere::get_molecular_absorption(float height, float wl) const
{
    float sigma_a = lut_lerp(ozone_cross_section_lut, wl) * 1e-4f; // m^2 / molecules
    // 1 Dobson = 2.6867e20 molecules / m^2
    float total_ozone = ozone_mean_monthly_dobson[_month] * 2.6867e20f; // molecules / m^-2
    float density = get_ozone_height_distribution(height) * total_ozone; // molecules / m^-2
    density /= 9e3f; // m^-3
    return sigma_a * density; // m^-1
}

float
GuimeraAtmosphere::get_ozone_height_distribution(float height) const
{
    if (height <= 9000.0f)
        return 9.0f / 210.0f;
    else if (height <= 18000.0f)
        return 14.0f / 210.0f;
    else if (height <= 27000.0f)
        return 111.0f / 210.0f;
    else if (height <= 36000.0f)
        return 64.0f / 210.0f;
    else if (height <= 45000.0f)
        return 6.0f / 210.0f;
    else if (height <= 54000.0f)
        return 6.0f / 210.0f;

    return 0.0f;
}
