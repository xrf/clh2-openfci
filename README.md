# clh2-openfci

**Quick links:** [releases](https://github.com/xrf/clh2-openfci/releases).

Utilities to tabulate the matrix elements using [OpenFCI](https://github.com/xrf/simen-openfci).

## Requirements

  - You'll need [LAPACK](http://netlib.org/lapack) in some form.  Depending on what exotic vendor library you happen to use, adjustments to the `Makefile` may be needed to compile.
  - You'll also need the [SQLite3](https://sqlite.org) library and its header files.

## Usage

Download and unpack the [latest release](https://github.com/xrf/clh2-openfci/releases) (or clone the repo), then run `make` to build all the tools.

To run the tools, start a custom shell session via:

```sh
make env SHELL="$SHELL"
```

Within this shell session, you have access to various tools:

  - `clh2of-simple-tabulate`: Tabulate matrix elements in simple binary format.
  - `clh2of-simple-dump`: Convert simple binary format to simple text format.
  - `clh2of-simple-mod`: Modify matrix elements in simple binary format.
  - `clh2of-tabulate`: Tabulate matrix elements in keyed SQLite3 format.
  - `clh2of-pack`: Convert keyed SQLite3 format to keyed binary format.
  - `clh2of-unpack`: Convert keyed binary format to keyed SQLite3 format.

**Known limitation:**  The endianness of binary formats produced by these tools depends on your current platform.

For a detailed explanation of the various formats, refer to the following documents:

  - [Simple formats](clh2of-simple-format.md)
  - [Keyed formats](clh2of-keyed-format.md)

Generally, you should stick to the simple format for simplicity sake.

### Auxiliary scripts

  - `./clh2of.py`: A basic Python wrapper around the matrix element calculator;
    useful for testing.  You can start an interactive session via:

    ```sh-session
    $ python -i ./clh2of.py
    ```

  - `./clh2of_table.py`: A tiny Python library for accessing matrix elements in keyed formats.

  - `./example.py`: An example that demonstrates the use of `clh2of_table.py`.
