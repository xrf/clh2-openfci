#!/usr/bin/env python
import ctypes, struct

_lib = ctypes.cdll.LoadLibrary("libclh2of.so")
_clh2of_init = _lib.clh2of_init
_clh2of_init.argtypes = [ctypes.c_int]
_clh2of_init.restype = ctypes.c_void_p
_clh2of_element = _lib.clh2of_element
_clh2of_element.argtypes = [ctypes.c_void_p]
_clh2of_element.restype = ctypes.c_double
_clh2of_free = _lib.clh2of_free
_clh2of_free.argtypes = [ctypes.c_void_p]
_clh2of_free.restype = None

class Clh2of(object):
    def __init__(self):
        self._ctx = _clh2of_init(-1)
    def element(self, n1, ml1, n2, ml2, n3, ml3, n4, ml4):
        ix = struct.pack("BbBbBbBb", n1, ml1, n2, ml2, n3, ml3, n4, ml4)
        return _clh2of_element(self._ctx, ix)
    def __del__(self):
        _clh2of_free(self._ctx)

_global_ctx = Clh2of()
element = _global_ctx.element
