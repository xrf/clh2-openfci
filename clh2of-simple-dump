#!/usr/bin/env python
#
# Reads tables in the following format and dumps them to standard output in a
# human-readable form:
#
#     ((<n:uint8> <ml:int8>){4} <value:float64>)*
#
import os, struct, sys

if len(sys.argv) != 2:
    sys.stderr.write("Usage: {0} <filename>\n"
                     .format(os.path.basename(sys.argv[0])))
    exit(1)

with open(sys.argv[1], "rb") as f:
    print("# n1\tml1\tn2\tml2\tn3\tml3\tn4\tml4\tvalue")
    while True:
        s = f.read(16)
        if not s:
            break
        n1, ml1, n2, ml2, n3, ml3, n4, ml4, value = \
            struct.unpack("@BbBbBbBbd", s)
        print("\t".join(map(str, [n1, ml1, n2, ml2, n3, ml3, n4, ml4, value])))
