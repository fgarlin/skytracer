# Skytracer

![Skytracer Example Output](https://raw.githubusercontent.com/fgarlin/skytracer/master/example_output.png)

**Skytracer** is a Monte Carlo spectral volumetric path tracer that renders the Earth's atmosphere in different conditions. It was developed as part of my Master's Thesis to obtain ground truth references that could be used to optimize a real-time approximation for atmosphere rendering.

The atmosphere model is described by Guimera et al. in ["A Physically-Based Spatio-Temporal Sky Model"](http://giga.cps.unizar.es/~ajarabo/pubs/Guimera2018spatio/downloads/Guimera2018spatio.pdf). In order to be as unbiased as possible, the volumetric path tracer uses delta tracking to find the next scattering/absorption event along the view ray, and [ratio tracking](https://cs.dartmouth.edu/wjarosz/publications/novak14residual.html) to estimate the transmittance.

A real-time approximation of the results generated by Skytracer can be found in this [Shadertoy](https://www.shadertoy.com/view/msXXDS).

## Building

This project is only known to work on Linux, but it probably builds on Windows and MacOS as long as you satisfy the required dependencies.

### Cloning the repo

Skytracer depends on [GLM (OpenGL Mathematics)](https://github.com/g-truc/glm), which is provided as a Git submodule. To clone the repository, including submodules, use:

```
git clone --recursive https://github.com/fgarlin/skytracer
```

Alternatively, if you didn't use the `--recursive` option when cloning the repo, you can run:

```
git submodule init
git submodule update
```

### Dependencies

The following dependencies are not bundled with Skytracer and must be installed before attempting to build the project:

* A compiler that supports C++17
* CMake >= 3.18
* zlib, required by TinyEXR to save EXR files
* [TBB (Threading Building Blocks)](https://github.com/oneapi-src/oneTBB), for multi-threading

### Compiling

This project is compiled like most CMake projects. For example, in Linux you can run:

``` sh
mkdir build && cd build
cmake ..
make -j8 # You can change 8 for the number of CPU cores in your system
```

## Usage

Once the project is compiled, the resulting binary can be used to generate EXR images of the Earth's atmosphere. The rendering configuration, atmospheric conditions and other settings can be changed through command-line arguments. Run `./skytracer --help` to see a list of possible parameters.

An example usage is shown below:

``` sh
./skytracer              \
    -w 512 -h 512        \  # We want a 512x512 image
    -l 490               \  # Sample the 490 nm wavelength
    -s 1024              \  # 1024 samples per pixel
    -a 1000              \  # Set the camera altitude to 1 km above sea level
    --aerosol-type urban \  # We want to simulate an urban environment
    --elevation 30       \  # Sun elevation angle of 30 degrees over the horizon
    example.exr             # Output to example.exr
```

### Rendering RGB images

Note that this renderer is monospectral, i.e. only a single wavelength is sampled during rendering, so the output image is grayscale. If you want to obtain an RGB image, several spectral samples have to be combined properly to obtain a correct result.

There are several ways to do spectral rendering. We provide a Python script on `scripts/render_rgb.py` that takes several uniformly distributed spectral samples and combines them using the [CIE color matching functions](https://en.wikipedia.org/wiki/CIE_1931_color_space#Color_matching_functions). The output are two image files:

* An EXR image containing HDR linear sRGB color values
* A PNG image, which is a tonemapped and gamma corrected version of the HDR image

A suitable [conda](https://conda.io) environment to run the script can be created and activated with:

``` sh
conda env create -f environment.yaml
conda activate skytracer
```

The script has its own set of command-line arguments, which can be listed with `python render_rgb.py --help`. Any arguments not recognized by the Python script will be passed along to the Skytracer executable.

## License

This code is released under the MIT license. See LICENSE for more details.
