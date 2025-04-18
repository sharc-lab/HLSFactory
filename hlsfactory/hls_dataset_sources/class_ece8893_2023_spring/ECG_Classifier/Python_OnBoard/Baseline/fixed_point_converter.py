
# Author: Rishov Sarkar

# Convert floating-point array to fixed-point
# Sample Usage:
# to_fixed_point(A_fixp, A_float, iwidth=3)
import numpy as np

def to_fixed_point(dst, src, *, width=None, iwidth, signed=True):
    if width is None:
        width = dst.dtype.itemsize * 8

    fwidth = width - iwidth
    epsilon = 1.0 / (2.0 ** fwidth)
    min_ = -1.0 * (2.0 ** (iwidth - 1)) if signed else 0.0
    max_ = (2.0 ** (iwidth - (1 if signed else 0))) - epsilon

    src = np.copy(src)
    src = src.reshape(dst.shape)
    src[src < min_] = min_
    src[src > max_] = max_
    if signed:
        src[src < 0] += (2 ** iwidth)
    dst[:] = np.around(src * (2.0 ** fwidth)).astype(dst.dtype)
    
# Convert fixed-point array back to floating point
# Sample Usage:
# B_float = from_fixed_point(B_fixp, iwidth=3)
def from_fixed_point(src, *, width=None, iwidth, signed=True):
    if width is None:
        width = src.dtype.itemsize * 8

    fwidth = width - iwidth
    src = np.array(src, dtype=np.int64)
    if signed:
        src[src >= (2 ** (width - 1))] -= (2 ** width)
    return src / (2.0 ** fwidth)