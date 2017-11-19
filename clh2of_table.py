#!/usr/bin/env python

def _orbital_index(n, ml):
    k = 2 * n + abs(ml)
    return (k * (k + 2) + ml) // 2

def _construct_key(n1, ml1, n2, ml2, n3, ml3, n4, ml4):
    '''Key construction function.'''
    p1 = _orbital_index(n1, ml1)
    p2 = _orbital_index(n2, ml2)
    p3 = _orbital_index(n3, ml3)
    p4 = _orbital_index(n4, ml4)
    if sorted((p1, p2)) > sorted((p3, p4)):
        p1, p2, p3, p4 = p3, p4, p1, p2
    if p1 > p2 or (p1 == p2 and p3 > p4):
        p1, p2, p3, p4 = p2, p1, p4, p3
    return p1 << 48 ^ p2 << 32 ^ p3 << 16 ^ p4

class TableSqlite(object):

    def __init__(self, filename):
        import sqlite3
        self._connection = sqlite3.connect(filename)
        self._cursor = self._connection.cursor()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def close(self):
        self._connection.close()

    def element(self, n1, ml1, n2, ml2, n3, ml3, n4, ml4):
        key = _construct_key(n1, ml1, n2, ml2, n3, ml3, n4, ml4)
        sql = "SELECT value FROM data WHERE key = ?"
        result = self._cursor.execute(sql, (key,)).fetchone()
        if result is not None:
            return result[0]

class TableBinary(object):

    def __init__(self, filename):
        import os
        self._file = open(filename, "rb")
        self._size = os.stat(filename).st_size // 16

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def close(self):
        self._file.close()

    def element(self, n1, ml1, n2, ml2, n3, ml3, n4, ml4):
        import struct
        key = _construct_key(n1, ml1, n2, ml2, n3, ml3, n4, ml4)
        imin = 0
        imax = self._size - 1
        i = imax // 2
        # locate element using binary search
        while True:
            self._file.seek(i * 16)
            k = struct.unpack("=Q", self._file.read(8))[0]
            if key == k:
                return struct.unpack("=d", self._file.read(8))[0]
            elif key < k:
                imax = i - 1
            else:
                imin = i + 1
            if imax < imin:
                break
            i = (imin + imax) // 2
