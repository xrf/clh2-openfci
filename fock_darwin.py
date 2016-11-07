import colorsys
from math import factorial
from numpy import exp, pi, sqrt
import matplotlib.pyplot as plt
import numpy as np
from scipy.special import genlaguerre

def colorize_complex(z):
    r = abs(z)
    arg = np.angle(z)
    h = (arg + pi) / (2 * pi) + 0.5
    l = r
    s = 1.0
    return np.array(np.vectorize(colorsys.hls_to_rgb)(h, l, s)).swapaxes(0, 2)

def radial_fock_darwin(n, m, r):
    return (sqrt(2 * factorial(n) / factorial(n + m)) *
            exp(-r ** 2 / 2) * r ** m * genlaguerre(n, m)(r ** 2))

def angular_orbital(ml, phi):
    return exp(1j * ml * phi) / sqrt(2 * pi)

def fock_darwin(n, ml, r, phi):
    return radial_fock_darwin(n, abs(ml), r) * angular_orbital(ml, phi)

def plot_orbital(n, ml, smax=5):
    x, y = np.meshgrid(np.linspace(-smax, smax, 200),
                       np.linspace(-smax, smax, 200))
    r = sqrt(x * x + y * y)
    phi = np.angle(x + 1j * y)
    z = colorize_complex(fock_darwin(n, ml, r, phi))
    plt.imshow(z)
    plt.show()

print(radial_fock_darwin(1, 2, 3))
print(angular_orbital(1, 2))

# objective: do some "manual" calculations with the aid of numpy
# compare them with OpenFCI for a few cases
# then regularize the interaction, modify OpenFCI, and check again
