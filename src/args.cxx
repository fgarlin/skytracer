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

#include "args.hxx"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

CommandLineArguments::CommandLineArguments()
{
}

void
CommandLineArguments::parse_args(int argc, char **argv)
{
    bool filename_given = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help") {
            print_help(argv[0]);
            exit(EXIT_SUCCESS);
        } else if (arg == "--width" || arg == "-w") {
            if (++i >= argc) {
                throw std::runtime_error("--width needs an argument");
            } else {
                width = std::stoi(argv[i]);
            }
        } else if (arg == "--height" || arg == "-h") {
            if (++i >= argc) {
                throw std::runtime_error("--height needs an argument");
            } else {
                height = std::stoi(argv[i]);
            }
        } else if (arg == "--tile-width" || arg == "-tw") {
            if (++i >= argc) {
                throw std::runtime_error("--tile-width needs an argument");
            } else {
                tile_width = std::stoi(argv[i]);
            }
        } else if (arg == "--tile-height" || arg == "-th") {
            if (++i >= argc) {
                throw std::runtime_error("--tile-height needs an argument");
            } else {
                tile_height = std::stoi(argv[i]);
            }
        } else if (arg == "--wavelength" || arg == "-l") {
            if (++i >= argc) {
                throw std::runtime_error("--wavelength needs an argument");
            } else {
                wavelength = std::stof(argv[i]);
            }
        } else if (arg == "--integrator" || arg == "-i") {
            if (++i >= argc) {
                throw std::runtime_error("--integrator needs an argument");
            } else {
                integrator = std::stoi(argv[i]);
            }
        } else if (arg == "--samples" || arg == "-s") {
            if (++i >= argc) {
                throw std::runtime_error("--samples needs an argument");
            } else {
                samples = std::stoi(argv[i]);
            }
        } else if (arg == "--camera" || arg == "-c") {
            if (++i >= argc) {
                throw std::runtime_error("--camera needs an argument");
            } else {
                camera = std::stoi(argv[i]);
            }
        } else if (arg == "--atmospheric-model") {
            if (++i >= argc) {
                throw std::runtime_error("--atmospheric-model needs an argument");
            } else {
                atmospheric_model = std::stoi(argv[i]);
            }
        } else if (arg == "--aerosol-type") {
            if (++i >= argc) {
                throw std::runtime_error("--aerosol-type needs an argument");
            } else {
                aerosol_type = std::string(argv[i]);
            }
        } else if (arg == "--list-aerosol-types") {
            list_aerosol_types();
            exit(EXIT_SUCCESS);
        } else if (arg == "--turbidity") {
            if (++i >= argc) {
                throw std::runtime_error("--turbidity needs an argument");
            } else {
                turbidity = std::stof(argv[i]);
            }
        } else if (arg == "--month") {
            if (++i >= argc) {
                throw std::runtime_error("--month needs an argument");
            } else {
                month = std::stof(argv[i]);
            }
        } else if (arg == "--max-order" || arg == "-o") {
            if (++i >= argc) {
                throw std::runtime_error("--max-order needs an argument");
            } else {
                max_order = std::stoi(argv[i]);
            }
        } else if (arg == "--only-ms") {
            only_ms = true;
        } else if (arg == "--albedo") {
            if (++i >= argc) {
                throw std::runtime_error("--albedo needs an argument");
            } else {
                albedo = std::stof(argv[i]);
            }
        } else if (arg == "--elevation") {
            if (++i >= argc) {
                throw std::runtime_error("--elevation needs an argument");
            } else {
                sun_elevation = std::stof(argv[i]);
            }
        } else if (arg == "--azimuth") {
            if (++i >= argc) {
                throw std::runtime_error("--azimuth needs an argument");
            } else {
                sun_azimuth = std::stof(argv[i]);
            }
        } else if (arg == "--eye-altitude" || arg == "-a") {
            if (++i >= argc) {
                throw std::runtime_error("--eye-altitude needs an argument");
            } else {
                eye_altitude = std::stof(argv[i]);
            }
        } else if (arg[0] != '-') {
            if (!filename_given) {
                filename = arg;
                filename_given = true;
            } else {
                throw std::runtime_error("Only one output filename allowed");
            }
        } else {
            throw std::runtime_error("Unknown option '" + arg + "'. "
                                     "Use --help to see all available options");
        }
    }
}

void
CommandLineArguments::print_help(const char *arg0) const
{
    std::cerr
        << "Usage: " << arg0 << " [OPTIONS] [FILENAME]\n\n"
        << "      --help                   Show this help message and quit\n"
        << "  -w, --width                  Set the image width (256 by default)\n"
        << "  -h, --height                 Set the image height (256 by default)\n"
        << " -tw, --tile-width             Tile width for multithreaded rendering (32 by default)\n"
        << " -th, --tile-height            Tile height for multithreaded rendering (32 by default)\n"
        << "  -l, --wavelength             Wavelength to sample in nanometers (550nm by default)\n"
        << "  -i, --integrator             Integrator to use (0=path tracer (default), 1=transmittance)\n"
        << "  -s, --samples                Number of path tracing samples per pixel (512 by default)\n"
        << "  -c, --camera                 Camera type (0=equirectangular, 1=fisheye (default))\n"
        << "      --atmospheric-model      Atmospheric model to use (0=Guimera (default))\n"
        << "      --aerosol-type           Aerosol type to use ('urban' by default)\n"
        << "      --list-aerosol-types     List all aerosol types\n"
        << "      --turbidity              Turbidity of the aerosols (1.0 by default)\n"
        << "      --month                  Month of the year 0 to 11 (0=January by default)\n"
        << "  -o, --max-order              Maximum scattering order (10000 by default). 1 corresponds to single scattering\n"
        << "      --only-ms                Only render multiple scattering (skip 1st scattering order)\n"
        << "      --albedo                 Set the ground albedo (0.3 by default)\n"
        << "      --elevation              Sun elevation angle in degrees (0=horizon (default), 90=zenith)\n"
        << "      --azimuth                Sun azimuth angle in degrees (0 by default)\n"
        << "  -a, --eye-altitude           Set the altitude of the camera above sea level in meters (0m by default)\n"
        << "\n"
        << std::flush;
}

void
CommandLineArguments::list_aerosol_types() const
{
    std::cerr
        << "\nAvailable aerosol types (use with --aerosol-type)\n"
        << "    none\n"
        << "    background\n"
        << "    desert-dust\n"
        << "    maritime-clean\n"
        << "    maritime-mineral\n"
        << "    polar-antarctic\n"
        << "    polar-artic\n"
        << "    remote-continental\n"
        << "    rural\n"
        << "    urban\n"
        << std::endl;
}
