# Simple formats (`clh2of-simple`) v2

*Changes since v1: Fixed incomplete canonicalization conditions.*

In this document, formal grammar is specified using [W3C EBNF notation](https://www.w3.org/TR/REC-xml/#sec-notation).

## Mathematical definition

The value of each entry is defined to be the given by the following integral:

    ⟨n1 ml1, n2 ml2 | V | n3 ml3, n4 ml4⟩ =
      ∬ φ[n1 ml1](r) φ[n2 ml2](r') (1/|r - r'|) φ[n3 ml3](r) φ[n4 ml4](r') dr dr'

where `φ[n ml]` is the 2-dimensional harmonic oscillator wave function with (zero-based) principal quantum number `n` and angular momentum projection `ml`.  The matrix elements are *not* antisymmetrized, because spin has not yet been taken into account.

Matrix elements that do not conserve `ml` are not included in the table.

## Binary format (`clh2of-simple-binary`)

A table consists of a sequence of [canonical](#canonicalization) matrix element entries.  In the binary format, the table is stored as a concatenation of all its entries with no separators.  Each entry is 16 bytes, thus a table with `N` entries would occupy exactly `16 × N` bytes.

    table ::= entry*

The order of entries within a table is not specified.

An entry consists of 9 fields.  The first 8 bytes contain the 8 quantum numbers, and the remaining 8 bytes contain the value of the matrix element as an IEEE 754 double-precision floating-point number.

    entry ::= n1 ml1 n2 ml2 n3 ml3 n4 ml4 value
    n1    ::= uint8
    ml1   ::= int8
    n2    ::= uint8
    ml2   ::= int8
    n3    ::= uint8
    ml3   ::= int8
    n4    ::= uint8
    ml4   ::= int8
    value ::= float64

It is recommended to use the `.dat` file extension for binary formats.

### Endianness

Little-endian order is recommended for general use.  Big-endian order may be used for transient storage of matrix elements on big-endian platforms, but are strongly discouraged for transmission or dissemination of matrix elements.

## Textual format (`clh2of-simple-text`)

On an abstract level, the textual format is structured in exactly the same way as the binary format.  The main difference is in the representation:

  - All numbers are presented in a human-readable form.

  - Each entry is stored as a single line, with the fields separated by horizontal whitespace (spaces and/or tabs).

  - There may exist empty lines that do not represent entries.  These lines consist of only horizontal whitespace.

        empty_line ::= hspace* newline

  - There may exist comment lines that do not represent entries.  These lines begin with the `#` character.

        comment_line ::= hspace* "#" non_newline* newline

It is recommended to use the `.txt` file extension for textual formats.

## Canonicalization

A matrix element for `(n1, ml1, n2, ml2, n3, ml3, n4, ml4)` is said to be **canonical** if and only if

~~~hs
(p1, p2) <= sort(p3, p4) && (p1, p3) <= (p2, p4)

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

Note that comparisons such as `(a, b) <= (c, d)` are performed using lexicographical ordering.

Non-canonical matrix elements can be recovered from canonical matrix elements through particle nondistinguishability,

    ⟨p1 p2| V |p3 p4⟩ = ⟨p2 p1| V |p4 p3⟩

and operator hermitivity,

    ⟨p1 p2| V |p3 p4⟩ = conj(⟨p3 p4| V |p1 p2⟩)

## Example

### C implementation of simple binary format

For simplicity, assume the platform is little-endian.  Conceptually, the binary format is structured like a contiguous array of the following structure:

~~~c
struct entry {
    uint8_t n1;
    int8_t ml1;
    uint8_t n2;
    int8_t ml2;
    uint8_t n3;
    int8_t ml3;
    uint8_t n4;
    int8_t ml4;
    double value;
};
~~~

The number of `struct entry` objects in the array can be inferred from the size of the file (`= file_size_in_bytes / 16`).  There is no padding.  Iterating over the file is straightforward in C or C++:

~~~c
struct entry ent;
while (fread(&ent, sizeof(ent), 1, file) == 1) {
    printf("<%i %i %i %i | V | %i %i %i %i> = %f\n",
           ent.n1, ent.ml1, ent.n2, ent.ml2, ent.n3, ent.ml3, ent.n4, ent.ml4,
           ent.value);
}
~~~
