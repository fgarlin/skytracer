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

#ifndef ARGS_HXX
#define ARGS_HXX

#include <string>

class CommandLineArguments final {
public:
    CommandLineArguments();

    void parse_args(int argc, char **argv);

    std::string filename = "out.exr";
    int width = 256;
    int height = 256;
    int tile_width = 32;
    int tile_height = 32;
    float wavelength = 550.0f;
    int integrator = 0;
    int samples = 512;
    int camera = 1;
    int atmospheric_model = 0;
    std::string aerosol_type = "urban";
    float turbidity = 1.0f;
    int month = 0;
    int max_order = 10000;
    bool only_ms = false;
    float albedo = 0.3f;
    float sun_elevation = 0.0f;
    float sun_azimuth = 0.0f;
    float eye_altitude = 0.0f;
private:
    CommandLineArguments(const CommandLineArguments &) = delete;
    CommandLineArguments &operator=(const CommandLineArguments &) = delete;

    void print_help(const char *arg0) const;
    void list_aerosol_types() const;
};

#endif // ARGS_HXX
