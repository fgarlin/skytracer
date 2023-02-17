import glob
import os
import numpy as np
import imageio
from scipy import interpolate, integrate


def linear_srgb_to_xyz(linear_srgb):
    return np.array([[0.4124564, 0.3575761, 0.1804375],
                     [0.2126729, 0.7151522, 0.0721750],
                     [0.0193339, 0.1191920, 0.9503041]]) @ linear_srgb.T;

def xyz_to_linear_srgb(xyz):
    return np.array([[ 3.2404542, -1.5371385, -0.4985314],
                     [-0.9692660,  1.8760108,  0.0415560],
                     [ 0.0556434, -0.2040259,  1.0572252]]) @ xyz.T;

def cmf_xyz_to_linear_srgb(cmf_xyz):
    """
    Transform the given CIE XYZ standard observer functions to linear sRGB.
    """
    lambdas = cmf_xyz[:, [0]]
    cmf_xyz_values = cmf_xyz[:, 1:]
    cmf_rgb_values = np.transpose(xyz_to_linear_srgb(cmf_xyz_values))
    return np.hstack((lambdas, cmf_rgb_values))

def resample_cmf(cmf, new_lambdas):
    """
    Resample a complete CMF for the given lambdas.
    @return A 2D array of size nx4 where n is the number of given lambdas and
            the first column is the lambdas array.
    """
    lambdas = cmf[:, 0]
    cmf_x   = cmf[:, 1]
    cmf_y   = cmf[:, 2]
    cmf_z   = cmf[:, 3]
    tck_x = interpolate.splrep(lambdas, cmf_x)
    tck_y = interpolate.splrep(lambdas, cmf_y)
    tck_z = interpolate.splrep(lambdas, cmf_z)
    new_cmf_x = interpolate.splev(new_lambdas, tck_x)
    new_cmf_y = interpolate.splev(new_lambdas, tck_y)
    new_cmf_z = interpolate.splev(new_lambdas, tck_z)
    new_cmf = np.vstack((new_lambdas, new_cmf_x, new_cmf_y, new_cmf_z)).T
    return new_cmf

def tristimulus_image_from_spectral_image(cmf, lambdas, spectral_img):
    """
    Compute a tristimulus image (XYZ, RGB, etc.) from a spectral image.
    """
    integrand1 = spectral_img * cmf[:, 1]
    integrand2 = spectral_img * cmf[:, 2]
    integrand3 = spectral_img * cmf[:, 3]
    c1 = integrate.simps(integrand1, lambdas, axis=2)
    c2 = integrate.simps(integrand2, lambdas, axis=2)
    c3 = integrate.simps(integrand3, lambdas, axis=2)
    return np.dstack([c1, c2, c3]).astype(np.float32)

def load_image_stack(directory):
    """
    Load several monospectral images from disk and convert them to a 3D numpy
    array, where the 3rd dimension is the wavelength.
    """
    files = sorted(filter(os.path.isfile, glob.glob(directory + "/*.exr")))
    num_files = len(files)

    image_stack = np.array([], dtype=np.float32)
    lambdas = np.zeros(num_files, dtype=np.float32)

    for i, filename in enumerate(files):
        img = imageio.imread(filename)
        image_stack = np.dstack([image_stack, img]) if image_stack.size else img
        wavelength = os.path.splitext(os.path.basename(filename))[0]
        lambdas[i] = wavelength

    return lambdas, image_stack

def load_resampled_cmf_from_csv(filename, lambdas):
    """
    Load the CIE color matching functions from a CSV file and resample the data
    according to the given wavelengths.
    """
    cmf = np.genfromtxt(filename, delimiter=',').astype(np.float32)
    return resample_cmf(cmf, lambdas)

def gamma_correct(linear):
    """
    Gamma correct a linear sRGB color.
    """
    a = 12.92 * linear
    b = 1.055 * (linear**(1.0/2.4)) - 0.055
    return np.where(linear <= 0.0031308, a, b)

def to_ldr(image, k):
    """
    Convert an HDR image to an LDR image with tonemapping and gamma correction.
    """
    image = 1.0 - np.exp(-k * image) # Tonemapping
    image = np.clip(image, 0.0, 1.0)
    image = gamma_correct(image)
    image = np.clip(image, 0.0, 1.0)
    image = image * 255
    return image.astype(np.uint8)
