.POSIX:
.SUFFIXES:

# if your LAPACK implementation requires unusual libraries (e.g. MKL), you
# will need to modify this variable accordingly:
liblapack=-llapack

# build flags
CPPFLAGS=-Idist/include
CFLAGS=$(allflags) -std=c99 -fPIC
CXXFLAGS=$(allflags) -std=c++03 -fPIC
allflags=-g -O2 -mtune=native -Wall -pedantic

all: \
    dist/bin/clh2of-tabulate \
    dist/bin/clh2of-pack \
    dist/bin/clh2of-unpack

clean:
	rm -fr dist

env: all
	PATH=dist/bin:$$PATH \
	    LIBRARY_PATH=dist/lib$${LIBRARY_PATH+:}$$LIBRARY_PATH \
	    LD_LIBRARY_PATH=dist/lib$${LD_LIBRARY_PATH+:}$$LD_LIBRARY_PATH \
	    C_INCLUDE_PATH=dist/include$${C_INCLUDE_PATH+:}$$C_INCLUDE_PATH \
	    CPLUS_INCLUDE_PATH=dist/include$${CPLUS_INCLUDE_PATH+:}$$CPLUS_INCLUDE_PATH \
	    $(SHELL)

.PHONY: all clean env

dist/bin/clh2of-tabulate: \
    dist/lib/libclh2of.so \
    dist/tmp/clh2of-tabulate.o
	mkdir -p dist/bin
	$(CC) -o $@ \
	    dist/tmp/clh2of-tabulate.o \
	    -Ldist/lib -lclh2of -lsqlite3

dist/bin/clh2of-pack: \
    clh2of-pack.c \
    utils.inl
	mkdir -p dist/bin
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ clh2of-pack.c -lsqlite3

dist/bin/clh2of-unpack: \
    clh2of-unpack.c \
    utils.inl
	mkdir -p dist/bin
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ clh2of-unpack.c -lsqlite3

dist/lib/libclh2of.so: \
    dist/tmp/clh2of.o \
    dist/tmp/openfci/src/manybody/gauss_tools.o \
    dist/tmp/openfci/src/quantumdot/QdotInteraction.o \
    dist/tmp/openfci/src/quantumdot/RadialPotential.o
	mkdir -p dist/lib
	$(CXX) -shared -Wl,-soname,libclh2of.so -o $@ \
	    dist/tmp/clh2of.o \
	    dist/tmp/openfci/src/manybody/gauss_tools.o \
	    dist/tmp/openfci/src/quantumdot/QdotInteraction.o \
	    dist/tmp/openfci/src/quantumdot/RadialPotential.o \
	    $(liblapack) -lpthread

dist/tmp/clh2of-tabulate.o: \
    clh2of-tabulate.c \
    dist/include/clh2of.h \
    utils.inl
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c \
	    clh2of-tabulate.c

dist/tmp/clh2of.o: \
    clh2of.cc \
    dist/include/clh2of.h \
    dist/tmp/openfci.ok
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c \
	    -Idist/tmp/openfci/src \
	    clh2of.cc

dist/include/clh2of.h: clh2of.h
	mkdir -p dist/include
	cp clh2of.h $@

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

dist/tmp/openfci.ok:
	. ./tools && get_openfci dist/tmp

dist/tmp/lpp.ok:
	. ./tools && get_lpp dist/tmp
