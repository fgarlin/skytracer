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

#include "renderer.hxx"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

#include <zlib.h>
#define TINYEXR_USE_MINIZ 0
#define TINYEXR_IMPLEMENTATION
#include "tinyexr.h"

#include "args.hxx"
#include "sampler.hxx"

using namespace glm;

namespace {

void
update_progress_bar(int count, int total)
{
    const int bar_width = 50;
    float progress = float(count) / total;
    int pos = progress * bar_width;
    std::cerr << "\rRendering [";
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) std::cerr << '=';
        else if (i == pos) std::cerr << '>';
        else std::cerr << ' ';
    }
    std::cerr << "] ";
    int percent = progress * 100.0f;
    if (percent < 10) std::cerr << "  ";
    else if (percent < 100) std::cerr << " ";
    std::cerr << percent << "%" << std::flush;
}

} // anonymous namespace

Renderer::Renderer(const CommandLineArguments &args) :
    _image_width(args.width),
    _image_height(args.height),
    _tile_width(args.tile_width),
    _tile_height(args.tile_height),
    _wavelength(args.wavelength),
    _samples_per_pixel(args.samples),
    _inv_image_size(1.0f / float(args.width), 1.0f / float(args.height))
{
    _buffer.resize(_image_width * _image_height);
    prepare_tiles();
    create_scene(args);
}

void
Renderer::render()
{
    using namespace std::chrono;

    tbb::blocked_range<size_t> range(0, _tiles.size());

    std::mutex progress_mutex;
    size_t progress_count = 0;
    update_progress_bar(0, _tiles.size());

    auto kernel = [&](const tbb::blocked_range<size_t> &range) {
        Sampler sampler(range.begin(), range.end());
        for (size_t i = range.begin(); i < range.end(); ++i) {
            render_tile(&sampler, _tiles[i]);

            std::scoped_lock lock(progress_mutex);
            ++progress_count;
            update_progress_bar(progress_count, _tiles.size());
        }
    };

    auto start = steady_clock::now();

    // Run the kernel
    tbb::parallel_for(range, kernel);

    auto end = steady_clock::now();

    auto elapsed = end - start;
    auto hrs = duration_cast<hours>(elapsed);
    auto mins = duration_cast<minutes>(elapsed - hrs);
    auto secs = duration_cast<seconds>(elapsed - hrs - mins);

    std::cerr << " (";
    if (hrs.count() != 0) std::cerr << hrs.count() << "h ";
    if (mins.count() != 0) std::cerr << mins.count() << "m ";
    std::cerr << secs.count() << "s)\n";
}

void
Renderer::write(const std::string &filename)
{
    const char *err = nullptr;
    int ret = SaveEXR(_buffer.data(), _image_width, _image_height, 1, 0,
                      filename.c_str(), &err);
    if (ret != TINYEXR_SUCCESS) {
        std::cerr << "Failed to write EXR image: " << err << std::endl;
        FreeEXRErrorMessage(err);
        return;
    }
    std::cerr << "Saved EXR image [ " << filename << " ]\n";
}

void
Renderer::create_scene(const CommandLineArguments &args)
{
    float aspect_ratio = float(_image_width) / float(_image_height);
    _scene = std::make_unique<Scene>();
    _scene->light = std::make_unique<Sun>(args.sun_elevation, args.sun_azimuth);
    switch(args.atmospheric_model) {
    case 0:
        _scene->atmosphere = std::make_unique<GuimeraAtmosphere>(args.month,
                                                                 args.turbidity,
                                                                 args.aerosol_type);
        break;
    default:
        throw std::runtime_error("Unknown atmospheric model");
    }
    switch (args.camera) {
    case 0:
        _scene->camera = std::make_unique<EquirectangularCamera>(args.eye_altitude);
        break;
    case 1:
        _scene->camera = std::make_unique<FisheyeCamera>(args.eye_altitude, aspect_ratio);
        break;
    default:
        throw std::runtime_error("Unknown camera mode");
    }
    switch (args.integrator) {
    case 0:
        _scene->integrator = std::make_unique<PathTracingIntegrator>(
            args.max_order, args.only_ms);
        break;
    case 1:
        _scene->integrator = std::make_unique<TransmittanceIntegrator>();
        break;
    default:
        throw std::runtime_error("Unknown integrator");
    }
    _scene->ground_albedo = args.albedo;
}

void
Renderer::prepare_tiles()
{
    if (!_tiles.empty())
        _tiles.clear();

    int x_tiles = (_image_width  + _tile_width  - 1) / _tile_width;
    int y_tiles = (_image_height + _tile_height - 1) / _tile_height;

    for (int j = 0; j < y_tiles; ++j) {
        for (int i = 0; i < x_tiles; ++i) {
            int x0 = i * _tile_width;
            int x1 = (i == x_tiles - 1) ? _image_width : x0 + _tile_width;

            int y0 = j * _tile_height;
            int y1 = (j == y_tiles - 1) ? _image_height : y0 + _tile_height;

            _tiles.push_back(Tile(x0, x1, y0, y1));
        }
    }
}

float
Renderer::render_pixel(Sampler *sampler, int x, int y, float wl) const
{
    vec2 pixel_coord{x, y};
    float accum = 0.0f;
    for (int i = 0; i < _samples_per_pixel; ++i) {
        // Get the normalized coordinates [0,1] of this sample
        vec2 uv = (pixel_coord + sampler->next_2d()) * _inv_image_size;
        // Sample a ray from the camera
        Ray ray;
        if (!_scene->camera->sample_ray(ray, uv))
            continue;
        // Compute the incident radiance
        accum += _scene->integrator->Li(_scene.get(), sampler, ray, wl);
    }
    accum /= _samples_per_pixel;
    return accum;
}

void
Renderer::render_tile(Sampler *sampler, const Tile &tile)
{
    for (int y = tile.y0; y < tile.y1; ++y) {
        for (int x = tile.x0; x < tile.x1; ++x) {
            float value = render_pixel(sampler, x, y, _wavelength);
            place_pixel(x, y, value);
        }
    }
}

void
Renderer::place_pixel(int x, int y, float value)
{
    size_t pixel_index = y * _image_width + x;
    _buffer[pixel_index] = value;
}
