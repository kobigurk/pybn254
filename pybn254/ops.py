import platform
import math
import ctypes
from ctypes import *
import os

from py_ecc.optimized_bn128 import add, multiply, pairing
from py_ecc.fields import optimized_bn128_FQ as FQ

lib_file = None
system_name = platform.system()
if system_name == 'Darwin':
    lib_file = 'libff.dylib'
elif system_name == 'Linux':
    lib_file = 'libff.so'
else:
    raise Exception('System %s not supported' % (system_name, ))

lib_file = os.path.join(os.path.dirname(__file__), lib_file)

cdll.LoadLibrary(lib_file)  
lib = CDLL(lib_file) 

def multiexp(bases, exponents):
    if exponents == []:
        return multiply(bases[0], 0)

    bases = bases[:len(exponents)]

    exponents_arr = arr_to_ctypes_arr([exponents[i] for i in range(len(exponents))])
    points_x_arr = arr_to_ctypes_arr(list(map(lambda p: int(p[0]), bases)))
    points_y_arr = arr_to_ctypes_arr(list(map(lambda p: int(p[1]), bases)))
    points_z_arr = arr_to_ctypes_arr(list(map(lambda p: int(p[2]), bases)))

    res_x = ctypes.c_buffer(32)
    res_y = ctypes.c_buffer(32)
    res_z = ctypes.c_buffer(32)

    lib.multiexp(exponents_arr, len(exponents), points_x_arr, points_y_arr, points_z_arr, len(bases), res_x, res_y, res_z);
    F = (FQ(int_from_bytes(res_x)), FQ(int_from_bytes(res_y)), FQ(int_from_bytes(res_z)))

    return F

def poly_fft(poly, inverse, coset):
    n = len(poly)
    logn = math.log(n, 2)
    if not logn.is_integer():
        poly = poly + [0] * ((1<<math.ceil(logn)) - len(poly))

    elements_arr = (ctypes.c_char_p * len(poly))()
    elements_arr[:] = list(map(lambda x: x.to_bytes(32, 'little'), poly))
    res_arr = ctypes.c_buffer(32*len(poly))
    lib.fft(elements_arr, len(elements_arr), res_arr, len(poly), inverse, coset);
    return [int_from_bytes(res_arr[32*i:32*(i+1)]) for i in range(len(poly))]

def multiply_polynomials(poly1,poly2):
    n1 = len(poly1)
    n2 = len(poly2)

    while len(poly1) < n1 + n2 - 1:
        poly1.append(0)

    while len(poly2) < n1 + n2 - 1:
        poly2.append(0)

    points1 = poly_fft(poly1[:], False, False)
    points2 = poly_fft(poly2[:], False, False)

    points = []
    for i in range(0,len(points1)):
        points.append( int(FQ(points1[i] * points2[i])) )

    poly = poly_fft(points[:], True, False)

    return poly

def arr_to_ctypes_arr(arr):
    ctypes_arr = (ctypes.c_char_p * len(arr))()
    ctypes_arr[:] = list(map(lambda x: x.to_bytes(32, 'little'), arr))
    return ctypes_arr

def int_from_bytes(i):
    return int.from_bytes(i, byteorder='little', signed=False)
