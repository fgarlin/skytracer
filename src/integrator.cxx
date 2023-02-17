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

#include "integrator.hxx"

#include <iostream>

#include "sampler.hxx"
#include "scene.hxx"

using namespace glm;

namespace {

/**
 * Return the distance between the ray origin and the first intersection with
 * a sphere centered in (0, 0, 0), or -1 if there is no intersection.
 * -1 is also returned if the ray is pointing away from the sphere (even if
 * there is an intersection).
 */
float
ray_sphere_intersection(const Ray &ray, float sr)
{
    vec3 oc = ray.o - EARTH_CENTER;
    float b = dot(oc, ray.d);
    float c = dot(oc, oc) - sr*sr;
    if (c > 0.0f && b > 0.0f) return -1.0f;
    float d = b*b - c;
    if (d < 0.0f) return -1.0f;
    if (d > b*b) return (-b+sqrtf(d));
    return (-b-sqrtf(d));
}

/**
 * Return an uniformly distributed vector on the unit sphere given two uniform
 * random variables.
 */
vec3
sample_uniform_sphere(const vec2 &s)
{
    float phi = M_TWO_PI * s.x;
    float cos_theta = s.y * 2.0f - 1.0f;
    float sin_theta = sqrtf(1.0f - cos_theta*cos_theta);
    return vec3(cosf(phi) * sin_theta, sinf(phi) * sin_theta, cos_theta);
}

/**
 * Return a cosine weighted vector on the unit hemisphere given two uniform
 * random variables.
 */
vec3
sample_cosine_weighted_hemisphere(const vec2 &sample)
{
    float phi = M_TWO_PI * sample.x;
    float cos_theta = sqrtf(sample.y);
    float sin_theta = sqrtf(1.0f - cos_theta*cos_theta);
    return vec3(cosf(phi) * sin_theta, sinf(phi) * sin_theta, cos_theta);
}

/**
 * Determine the next interaction point (scattering or absorption event) along
 * a ray inside the atmospheric medium using delta tracking.
 */
float
sample_interaction(const Atmosphere *atmosphere, Sampler *sampler,
                   const Ray &ray, float t_max, float wl, vec3 &p)
{
    float majorant = atmosphere->get_max_extinction(wl);
    float t = 0.0f;
    do {
        t -= logf(1.0f - sampler->next_1d()) / majorant;
        if (t >= t_max)
            break;
        p = ray.o + ray.d * t;
        float extinction = atmosphere->get_extinction(p, wl);
        if (sampler->next_1d() < fmaxf(0.0f, extinction / majorant))
            return t;
    } while(true);
    return -1.0f;
}

/**
 * Compute the transmittance along a ray segment with ratio tracking from
 * Nóvak et al. (2014).
 */
float
transmittance(const Atmosphere *atmosphere, Sampler *sampler,
              const Ray &ray, float t_max, float wl)
{
    float majorant = atmosphere->get_max_extinction(wl);
    float Tr = 1.0f;
    float t = 0.0f;
    do {
        t -= logf(1.0f - sampler->next_1d()) / majorant;
        if (t >= t_max)
            break;
        vec3 p = ray.o + ray.d * t;
        float extinction = atmosphere->get_extinction(p, wl);
        Tr *= 1.0f - fmaxf(0.0f, extinction / majorant);
    } while(true);
    return Tr;
}

float
sample_background(const Ray &ray, float wl)
{
    // We don't simulate deep space or even the Sun, so just return black
    return 0.0f;
}

float
scene_intersect(const Ray &ray, bool &intersected_earth)
{
    float t_max;
    intersected_earth = false;
    float atmos_t = ray_sphere_intersection(ray, ATMOSPHERE_RADIUS);
    // Check if the ray's origin is outside the atmosphere
    if (ray.o.z >= ATMOSPHERE_THICKNESS) {
        // We are outside the atmosphere
        if (atmos_t < 0.0f) {
            // No intersection with the atmosphere, looking into outer space
            t_max = -1.0f;
        } else {
            // Displace the ray's origin to the point of intersection with the
            // atmosphere.
            Ray new_ray(ray.o + ray.d * atmos_t, ray.d);
            float earth_t = ray_sphere_intersection(ray, EARTH_RADIUS);
            if (earth_t < 0.0f) {
                // No intersection with the Earth, so the ray must exit through
                // the atmosphere.
                t_max = ray_sphere_intersection(ray, ATMOSPHERE_RADIUS);
            } else {
                intersected_earth = true;
                t_max = earth_t;
            }
        }
    } else {
        // We are inside the atmosphere
        float earth_t = ray_sphere_intersection(ray, EARTH_RADIUS);
        if (earth_t < 0.0f) {
            // No intersection with the Earth, so the ray must exit through
            // the atmosphere.
            t_max = atmos_t;
        } else {
            intersected_earth = true;
            t_max = earth_t;
        }
    }
    return t_max;
}

void
sample_sun(const Scene *scene, Sampler *sampler, const vec3 &p,
           float wl, vec3 &shadow_ray_dir, float &beam_transmittance,
           float &L)
{
    L = scene->light->sample(sampler->next_2d(), shadow_ray_dir, wl);
    Ray shadow_ray(p, shadow_ray_dir);
    float earth_t = ray_sphere_intersection(shadow_ray, EARTH_RADIUS);
    if (earth_t < 0.0f) {
        float t = ray_sphere_intersection(shadow_ray, ATMOSPHERE_RADIUS);
        beam_transmittance = transmittance(scene->atmosphere.get(),
                                           sampler, shadow_ray, t, wl);
    } else {
        beam_transmittance = 0.0f;
    }
}

} // anonymous namespace

//------------------------------------------------------------------------------

float
TransmittanceIntegrator::Li(const Scene *scene, Sampler *sampler,
                            const Ray &ray, float wl)
{
    bool intersected_earth;
    float t_max = scene_intersect(ray, intersected_earth);
    if (t_max < 0.0f) {
        return 0.0f;
    }
    return transmittance(scene->atmosphere.get(), sampler, ray, t_max, wl);
}

//------------------------------------------------------------------------------

PathTracingIntegrator::PathTracingIntegrator(int max_order, bool only_ms) :
    _max_order(max_order),
    _only_ms(only_ms)
{
}

float
PathTracingIntegrator::Li(const Scene *scene, Sampler *sampler,
                          const Ray &ray_, float wl)
{
    const Atmosphere *atmosphere = scene->atmosphere.get();
    const LightSource *light = scene->light.get();

    Ray ray = ray_;
    float L = 0.0f;
    float throughput = 1.0f;

    for (int order = 1; order <= _max_order; ++order) {
        bool intersected_earth;
        float t_max = scene_intersect(ray, intersected_earth);
        if (t_max < 0.0f) {
            // No intersection with the atmosphere or the Earth. Add the
            // background and terminate the ray.
            L += sample_background(ray, wl);
            break;
        }

        vec3 interaction_point;
        float t = sample_interaction(atmosphere, sampler, ray, t_max,
                                     wl, interaction_point);
        if (t < 0.0f) {
            // We didn't find an interaction point inside the given ray segment
            if (!intersected_earth) {
                // Ray exited the atmosphere, add contribution from the
                // background and terminate the path.
                L += sample_background(ray, wl);
                break;
            } else {
                // Surface interaction
                //--------------------------------------------------------------
                // The only surface we can interact with is the Earth itself,
                // which is modelled as a perfectly diffuse sphere with a
                // configurable albedo.
                // We do not add the single scattering contribution from the
                // ground. If we did, we would get an ugly grey surface. We
                // prefer to leave it black and only add the multiple scattering
                // contribution that affects the sky's color.

                float bsdf = scene->ground_albedo * M_INV_PI;

                vec3 shading_point = ray.o + ray.d * t_max;
                vec3 n = normalize(shading_point - EARTH_CENTER);
                // To avoid self-intersection due to floating point precision
                shading_point += n;

                vec3 shadow_ray_dir;
                float beam_transmittance, sun_L;
                sample_sun(scene, sampler, shading_point, wl,
                           shadow_ray_dir, beam_transmittance, sun_L);
                float ndotl = dot(n, shadow_ray_dir);
                if (!_only_ms || order > 1) {
                    L += throughput * sun_L * bsdf * beam_transmittance * ndotl;
                }

                // Accumulate the weight
                throughput *= bsdf;

                // Reflection ray
                vec3 wi = sample_cosine_weighted_hemisphere(sampler->next_2d());
                // Create a local coordinate frame on the shading point
                vec3 s, t;
                if (fabsf(n.x) > fabsf(n.y)) {
                    float inv_len = 1.0f / sqrtf(n.x * n.x + n.z * n.z);
                    t = vec3(n.z * inv_len, 0.0f, -n.x * inv_len);
                } else {
                    float inv_len = 1.0f / sqrtf(n.y * n.y + n.z * n.z);
                    t = vec3(0.0f, n.z * inv_len, -n.y * inv_len);
                }
                s = cross(t, n);
                // Transform wi to the world frame
                wi = normalize(s * wi.x + t * wi.y + n * wi.z);

                // Update the next ray
                ray = Ray(shading_point, wi);
            }
        } else {
            // Medium interaction
            //------------------------------------------------------------------

            float scattering_albedo = atmosphere->get_scattering_albedo(
                interaction_point, wl);

            // Russian roulette to determine the collision event type
            if (sampler->next_1d() < scattering_albedo) {
                // Scattering event

                // Perform Next-Event Estimation by tracing a shadow ray to the Sun
                vec3 shadow_ray_dir;
                float beam_transmittance, sun_L;
                sample_sun(scene, sampler, interaction_point, wl,
                           shadow_ray_dir, beam_transmittance, sun_L);
                float phase = atmosphere->phase_eval(
                    interaction_point, sampler->next_1d(), -ray.d, shadow_ray_dir, wl);
                if (!_only_ms || order > 1) {
                    L += throughput * sun_L * phase * beam_transmittance
                        * scattering_albedo;
                }

                vec3 wi = sample_uniform_sphere(sampler->next_2d());

                // Not needed when using uniform sampling
                // float bsdf = phase_eval(next_1d(rng_state), interaction_point,
                //                         -ray.d, wi, wl);
                // throughput *= bsdf;

                // Update the next ray
                ray = Ray(interaction_point, wi);
            } else {
                // Absorption event, so terminate the path
                break;
            }
        }

        // Russian roulette to terminate the path early if the throughput is
        // low enough.
        if (order > 5) {
            float q = fmaxf(0.05f, 1.0f - throughput);
            if (sampler->next_1d() < q)
                break;
            throughput /= 1.0f - q;
        }

        // End the path if we lost all energy (should never be reached anyway)
        if (throughput <= 0.0f) {
            break;
        }
    }

    return L;
}
