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

#ifndef RENDERER_HXX
#define RENDERER_HXX

#include <memory>
#include <vector>

#include "scene.hxx"

class CommandLineArguments;
class Sampler;

class Renderer final {
public:
    struct Tile {
        Tile(int x0_, int x1_, int y0_, int y1_)
            : x0(x0_), x1(x1_), y0(y0_), y1(y1_) {}
        int x0, x1, y0, y1;
    };

    Renderer(const CommandLineArguments &args);

    void render();
    void write(const std::string &filename);
private:
    void create_scene(const CommandLineArguments &args);
    void prepare_tiles();
    float render_pixel(Sampler *sampler, int x, int y, float wl) const;
    void render_tile(Sampler *sampler, const Tile &tile);
    void place_pixel(int x, int y, float value);

    int _image_width, _image_height;
    int _tile_width,  _tile_height;
    float _wavelength;
    int _samples_per_pixel;
    glm::vec2 _inv_image_size;

    std::vector<float> _buffer;

    std::vector<Tile> _tiles;

    std::unique_ptr<Scene> _scene;
};

#endif // RENDERER_HXX
