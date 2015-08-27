# clh2-openfci

**Quick links:** [releases][rl].

A set of tools to tabulate the matrix elements calculated via [OpenFCI][of].

## Requirements

You need to have some form of [LAPACK](http://netlib.org/lapack) installed.
Depending on what exotic vendor library you happen to use, you may need to
adjust the `Makefile` to make it compile.

## Usage

Download and unpack the [latest release][rl] (or clone the repo), then run:

```sh
make env SHELL="$SHELL"
```

Within this session, you have access to the following tools:

  - `clh2of-tabulate`: Calculate and store matrix elements in SQLite format.
  - `clh2of-pack`: Convert matrix elements in SQLite format to binary format.
  - `clh2of-unpack`: Reverse of `clh2of-pack`.
  - `clh2of.py`: A basic Python wrapper around the matrix element calculator;
    useful for testing and interactive evaluation.

Matrix elements can be read in using standard tools.  The SQLite format is
easier to use in a high-level language like Python, but it is less compact.
The native binary format is a bit more difficult to use as you will need to
search the file for each element using I/O operations (or use memory mapping
if you have enough memory).

An example of how to access these matrix elements can be found in the
`example.py`, which makes use of `clh2of_table.py`.

## Specification of the matrix element formats

### 1. SQLite format

The schema is given by:

```sql
CREATE TABLE data (key INTEGER PRIMARY KEY, value REAL);
```

where `key` is defined in Section 3 and `value` is the value of the matrix
element.

### 2. Binary format

Matrix elements are stored as an undelimited sequence of rows, each containing
two cells in the following form:

    <key:Int64> <value:Float64>

where `key` is defined in Section 3 and `value` is the value of the matrix
element.  The cells are stored the *native representation*, which means that
these matrix elements may not portable across different platforms.

Hence, each row is 16 bytes (128 bits).  The rows are stored in ascending
order by `key`.

### 3. Key function

A key is constructed from the indices of the matrix element

    < n1, ml1; n2, ml2 | V | n3, ml3; n4, ml4 > =
      Integral[d r] Integral[d r']
        phi[n1, ml1](r) phi[n2, ml2](r')
        phi[n3, ml3](r) phi[n4, ml4](r')

Firstly, each pair `n` and `ml` is fused using the function:

    f(n, ml) = (k * (k + 2) + ml) / 2
      where k = 2 * n + abs(ml)

(The division by two always results in an integer.)

Then, the fused pairs `p = f(n, ml)` are rearranged such that

    (r1, r2) ≤ sort(r3, r4) && r1 ≤ r2

is satisfied; here, we define

    sort(r3, r4) =
      if r3 ≤ r4
      then (r3, r4)
      else (r4, r3)

and `(r1, r2, r3, r4)` to be some permutation of `(p1, p2, p3, p4)` generated
from some combination of `(1 2) (3 4)` and/or `(1 3) (2 4)`.

Finally, each `r`, treated as a 16-bit integer, is bitwise concatenated into a
64-bit integer:

    key =
      left_shift(r1, 48) +
      left_shift(r2, 32) +
      left_shift(r3, 16) +
                 r4

Note that the indices nearer to the left are stored in the higher bits.  Since
most architectures are little-endian, this means the indices are often stored
in reverse order.

A concrete implementation of the key function can be found in as
`clh2of_table.py`, where it is called `_construct_key`.

[rl]: https://github.com/xrf/clh2-openfci/releases
[of]: https://github.com/xrf/simen-openfci
