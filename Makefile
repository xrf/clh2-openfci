.POSIX:
.SUFFIXES:

# if your LAPACK implementation requires unusual libraries (e.g. MKL), you
# will need to modify this variable accordingly:
liblapack=-llapack

# installation prefix
PREFIX=/usr/local

# build flags
CPPFLAGS=-Iinclude
CFLAGS=$(allflags) -std=c99
CXXFLAGS=$(allflags) -std=c++03
allflags=-g -O2 -mtune=native -Wall -pedantic
libpthread=-lpthread

all: dist/bin/clh2-openfci

clean:
	rm -fr dist

install: all
	install -Dm755 dist/bin/clh2-openfci \
	    $(DESTDIR)$(PREFIX)/bin/clh2-openfci

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/clh2-openfci

.PHONY: all clean install uninstall

dist/bin/clh2-openfci: \
    dist/tmp/main.o \
    dist/tmp/coulomb_ho2d/dist/tmp/protocol.o \
    dist/tmp/coulomb_ho2d/dist/tmp/util.o \
    dist/tmp/openfci/src/manybody/gauss_tools.o \
    dist/tmp/openfci/src/quantumdot/QdotInteraction.o \
    dist/tmp/openfci/src/quantumdot/RadialPotential.o
	mkdir -p dist/bin
	$(CXX) -o $@ \
	    dist/tmp/main.o \
	    dist/tmp/coulomb_ho2d/dist/tmp/protocol.o \
	    dist/tmp/coulomb_ho2d/dist/tmp/util.o \
	    dist/tmp/openfci/src/manybody/gauss_tools.o \
	    dist/tmp/openfci/src/quantumdot/QdotInteraction.o \
	    dist/tmp/openfci/src/quantumdot/RadialPotential.o \
	    $(liblapack) $(libpthread)

dist/tmp/main.o: \
    main.cc \
    dist/tmp/coulomb_ho2d.ok \
    dist/tmp/openfci.ok
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c \
	    -Idist/tmp/coulomb_ho2d/include \
	    -Idist/tmp/coulomb_ho2d/src \
	    -Idist/tmp/openfci/src \
	    main.cc

dist/tmp/coulomb_ho2d/dist/tmp/util.o: dist/tmp/coulomb_ho2d.ok
	cd dist/tmp/coulomb_ho2d && $(MAKE) dist/tmp/util.o

dist/tmp/coulomb_ho2d/dist/tmp/protocol.o: dist/tmp/coulomb_ho2d.ok
	cd dist/tmp/coulomb_ho2d && $(MAKE) dist/tmp/protocol.o

dist/tmp/openfci/src/manybody/gauss_tools.o: \
    dist/tmp/lpp.ok \
    dist/tmp/openfci.ok
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c \
	    dist/tmp/openfci/src/manybody/gauss_tools.cc

dist/tmp/openfci/src/quantumdot/QdotInteraction.o: \
    dist/tmp/lpp.ok \
    dist/tmp/openfci.ok
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c \
	    dist/tmp/openfci/src/quantumdot/QdotInteraction.cc

dist/tmp/openfci/src/quantumdot/RadialPotential.o: \
    dist/tmp/lpp.ok \
    dist/tmp/openfci.ok
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c \
	    dist/tmp/openfci/src/quantumdot/RadialPotential.cc

dist/tmp/coulomb_ho2d.ok:
	. ./tools && get_coulomb_ho2d dist/tmp

dist/tmp/openfci.ok:
	. ./tools && get_openfci dist/tmp

dist/tmp/lpp.ok:
	. ./tools && get_lpp dist/tmp
