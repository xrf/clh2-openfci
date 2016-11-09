# Table format

## Specification

There's 3 important things to know:

  - [Data layout](#data-layout)
  - [Mathematical value](#mathematical-value)
  - [Deduplication](#deduplication)

### Data layout

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

## Mathematical value

The matrix elements are computed from the integral:

    ⟨(n1, ml1), (n2, ml2) | V | (n3, ml3), (n4, ml4)⟩ =
      ∫[d r] ∫[d r'] φ[n1, ml1](r) φ[n2, ml2](r') φ[n3, ml3](r) φ[n4, ml4](r')

where `φ` are 2-dimensional harmonic oscillator states.  The matrix elements
are *not* antisymmetrized, because spin has not yet been taken into account.
Matrix elements that do not conserve `ml` are not included.

## Deduplication

All matrix elements in the file are *canonical*, as defined below.  Non-canonical matrix elements are intentionally absent from the file as they can be obtained from an equivalent canonical matrix element by making use of particle nondistinguishability and operator hermitivity.  This condition reduces the size of the matrix element file by roughly 1/4.

A matrix element for `(n1, ml1, n2, ml2, n3, ml3, n4, ml4)` is said to be **canonical** if and only if

~~~hs
(p1, p2) <= sort(p3, p4) && p1 <= p2

where

  sort(p3, p4) =
    if p3 <= p4
    then (p3, p4)
    else (p4, p3)

  p1 = f(n1, ml1)
  p2 = f(n2, ml2)
  p3 = f(n3, ml3)
  p4 = f(n4, ml4)

  f(n, ml) = (k * (k + 2) + ml) / 2
      where k = 2 * n + abs(ml)
~~~

## Example

### C implementation

The format is "raw binary", meaning you read it as if you are reading directly from memory.(*)  Conceptually, it is structured as if it is a contiguous array of `struct entry` where

~~~c
struct entry {

    // quantum numbers (alternating 8-bit unsigned and signed integers)
    uint8_t n1;
    int8_t ml1;
    uint8_t n2;
    int8_t ml2;
    uint8_t n3;
    int8_t ml3;
    uint8_t n4;
    int8_t ml4;

    // value of the matrix element (IEEE 754 double-precision floating-point)
    double value;

};
~~~

The number of `struct entry` objects in the array is implied by the size of the file (= file_size / (16 bytes)).  There is no padding.  Iterating over the file should be straightforward in C or C++:

~~~c
struct entry ent;
while (fread(&ent, sizeof(ent), 1, file) == 1) {
    printf("matrix element (%i %i %i %i %i %i %i %i) = %f\n",
           ent.n1, ent.ml1, ent.n2, ent.ml2, ent.n3, ent.ml3, ent.n4, ent.ml4,
           ent.value);
}
~~~

(*) I'm making certain assumptions about endianness and integer representation here. Assume little-endian and 2's complement, which is the de facto for x86-64 platforms.  Certainly, the file format as describe wouldn't work if you try to read from a more exotic arch like PowerPC big-endian.
