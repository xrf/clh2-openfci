# clh2-openfci

**Quick links:** [releases][rl].

This is a tabulation provider that uses [OpenFCI][of] to calculate matrix
elements.  It is a specialized *plug-in* for the [clh2 library][cl], not meant
to be invoked directly.  While you could conceivably communicate through the
protocol directly, it's much easier to communicate via the clh2 library.

## Requirements

You need to have some form of [LAPACK](http://netlib.org/lapack) installed.
Depending on what exotic vendor library you happen to use, you may need to
adjust the `Makefile` to make it compile.

## Installation

Download and unpack the [latest release][rl], then run:

    make PREFIX=/usr/local install

Replace `/usr/local` with wherever you want it to be installed.

[rl]: https://github.com/xrf/clh2-openfci/releases
[cl]: https://github.com/xrf/coulomb_ho2d
[of]: https://github.com/xrf/simen-openfci
