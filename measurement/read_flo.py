import struct
import numpy as np
import sys


TAG_FLOAT = 202021.25

def read_flo(filename):
    f = open(filename, 'rb')
    tag, = struct.unpack('f', f.read(4))
    width, = struct.unpack('i', f.read(4))
    height, = struct.unpack('i', f.read(4))
    
    nbands = 2

    tmp = f.read(4*width*nbands*height)
    A = np.fromstring(tmp, dtype='float32')

    A = A.reshape((height, width, nbands))

    return A
