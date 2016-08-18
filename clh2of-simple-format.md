# Table format

The table is stored in following format:

    (<n1:uint8> <ml1:int8>
     <n2:uint8> <ml2:int8>
     <n3:uint8> <ml3:int8>
     <n4:uint8> <ml4:int8>
     <value:float64>)*

which can be denoted more compactly as:

    ((<n:uint8> <ml:int8>){4} <value:float64>)*

Explanation:

  - The file contains a repeating stream of 16-byte segments.  Each segment
    stores the quantum numbers in the first 8 bytes and the value of the
    matrix element in the remaining 8 bytes.
  - The 1st byte of a segment contains the `n1` quantum number, stored as an
    8-bit unsigned integer.
  - The 2nd byte of a segment contains the `ml1` quantum number, stored as an
    8-bit signed integer.
  - The 3rd byte of a segment contains the `n2` quantum number, stored as an
    8-bit unsigned integer.
  - The 4th byte of a segment contains the `ml2` quantum number, stored as an
    8-bit signed integer.
  - The 5th byte of a segment contains the `n3` quantum number, stored as an
    8-bit unsigned integer.
  - The 6th byte of a segment contains the `ml3` quantum number, stored as an
    8-bit signed integer.
  - The 7th byte of a segment contains the `n4` quantum number, stored as an
    8-bit unsigned integer.
  - The 8th byte of a segment contains the `ml4` quantum number, stored as an
    8-bit signed integer.
  - The remaining 8 bytes store the value of the matrix element as a 64-bit
    little-endian IEEE 754 floating-point number.

The matrix elements are computed from the integral:

    ⟨(n1, ml1), (n2, ml2) | V | (n3, ml3), (n4, ml4)⟩ =
      ∫[d r] ∫[d r'] φ[n1, ml1](r) φ[n2, ml2](r') φ[n3, ml3](r) φ[n4, ml4](r')

where `φ` are 2-dimensional harmonic oscillator states.  The matrix elements
are *not* antisymmetrized, because spin has not yet been taken into account.
