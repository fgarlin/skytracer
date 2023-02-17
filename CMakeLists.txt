cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(skytracer)

set(CMAKE_BUILD_TYPE Release)

find_package(TBB REQUIRED)
find_package(ZLIB REQUIRED)

set(SOURCES
  src/aerosol.cxx
  src/aerosol.hxx
  src/args.cxx
  src/args.hxx
  src/atmosphere.cxx
  src/atmosphere.hxx
  src/camera.cxx
  src/camera.hxx
  src/common.cxx
  src/common.hxx
  src/integrator.cxx
  src/integrator.hxx
  src/lightsource.cxx
  src/lightsource.hxx
  src/lut.cxx
  src/lut.hxx
  src/main.cxx
  src/phase.cxx
  src/phase.hxx
  src/random.hxx
  src/renderer.cxx
  src/renderer.hxx
  src/sampler.hxx
  src/tinyexr.h
  )

add_executable(skytracer ${SOURCES})
# Use C++17
target_compile_features(skytracer PRIVATE cxx_std_17)

add_subdirectory(3rdparty/glm EXCLUDE_FROM_ALL)

target_link_libraries(skytracer
  PRIVATE glm::glm
  PRIVATE TBB::tbb
  PRIVATE ZLIB::ZLIB)

target_include_directories(skytracer
  PUBLIC ${GLM_INCLUDE_DIRS})
