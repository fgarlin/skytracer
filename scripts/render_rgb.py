#!/usr/bin/env python

import argparse
import subprocess
import sys
import tempfile

from spectral_util import *


def render_rgb_image(executable_path, cmf_path, args, wavelength_array):
    # Save monospectral images to a temporary directory
    with tempfile.TemporaryDirectory() as tmp_dir:
        for i, wavelength in enumerate(wavelength_array):
            print("Wavelength = " + str(wavelength) + " nm, " +
                  str(i+1) + "/" + str(len(wavelength_array)))
            tmp_filename = tmp_dir + "/" + str(wavelength) + ".exr"
            call = [executable_path, "-l", str(wavelength)]
            call.extend(args)
            call.append(tmp_filename)
            proc = subprocess.run(call)

        # Load all images from the temporal directory
        lambdas, image_stack = load_image_stack(tmp_dir)
        # Load the CMF and convert it to linear sRGB from XYZ
        cmf = load_resampled_cmf_from_csv(cmf_path, lambdas)
        cmf = cmf_xyz_to_linear_srgb(cmf)
        # Uniform spectral sampling
        image = tristimulus_image_from_spectral_image(cmf, lambdas, image_stack)
        return image

def main():
    ignored_args = ["--list-aerosol-types"]
    if any(x in sys.argv[1:] for x in ignored_args):
        print("You have used a CLI argument that makes Skytracer not output to an image. Exiting...")
        exit(1)

    default_cmf_file  = os.path.join(sys.path[0], "lin2012xyz2e_1_7sf.csv")
    default_exec_file = os.path.join(sys.path[0], "..", "build", "skytracer")

    parser = argparse.ArgumentParser(
        add_help=False,
        description="Compute an sRGB image (EXR and PNG) using uniform spectral sampling.",
        epilog="IMPORTANT NOTE: All arguments that are not listed here will be passed directly to the skytracer executable. This makes it possible to configure all skytracer parameters using the CLI arguments for this Python script.")
    parser.add_argument("--help",
                        action="help",
                        help="show this help message and exit")
    parser.add_argument("--output", type=str,
                        default="out",
                        help="Output image filename")
    parser.add_argument("--exec", type=str,
                        default=default_exec_file,
                        help="Path to the skytracer executable")
    parser.add_argument("--cmf", type=str,
                        default=default_cmf_file,
                        help="Path to the CSV file containing the tabulated values for the CIE color matching functions")
    parser.add_argument("--begin", type=int,
                        default=390,
                        help="Left bound of the wavelength interval to sample")
    parser.add_argument("--end", type=int,
                        default=781,
                        help="Right bound of the wavelength interval to sample")
    parser.add_argument("--step", type=int,
                        default=10,
                        help="Step size for the wavelength interval")
    parser.add_argument("--exposure", type=float,
                        default=0.05,
                        help="Exposure setting for the tonemapper")
    args, skytracer_args = parser.parse_known_args()

    wavelength_array = range(args.begin, args.end, args.step)
    image = render_rgb_image(args.exec, args.cmf, skytracer_args, wavelength_array)

    # Tonemap and gamma correct to obtain an LDR image
    ldr_image = to_ldr(image, args.exposure)

    # Write them to disk
    out_exr = args.output + ".exr"
    out_ldr = args.output + ".png"
    imageio.imwrite(out_exr, image)
    imageio.imwrite(out_ldr, ldr_image, format="png")
    print("Wrote RGB image to [ " + out_exr + " ] and [ " + out_ldr + " ]")


if __name__ == "__main__":
    main()
