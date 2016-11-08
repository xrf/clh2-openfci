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
    dist/bin/clh2of-pack \
    dist/bin/clh2of-simple-add \
    dist/bin/clh2of-simple-tabulate \
    dist/bin/clh2of-tabulate \
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

install: all
	install -Dm644 clh2of.h $(DESTDIR)$(PREFIX)/include/clh2of.h
	install -Dm755 clh2of-simple-dump.py $(DESTDIR)$(PREFIX)/bin/clhof-simple-dump.py
	install -Dm755 dist/bin/clh2of-pack $(DESTDIR)$(PREFIX)/bin/clh2of-pack
	install -Dm755 dist/bin/clh2of-simple-add $(DESTDIR)$(PREFIX)/bin/clh2of-simple-add
	install -Dm755 dist/bin/clh2of-simple-tabulate $(DESTDIR)$(PREFIX)/bin/clh2of-simple-tabulate
	install -Dm755 dist/bin/clh2of-tabulate $(DESTDIR)$(PREFIX)/bin/clh2of-tabulate
	install -Dm755 dist/bin/clh2of-unpack $(DESTDIR)$(PREFIX)/bin/clh2of-unpack
	install -Dm755 dist/lib/libclh2of.so $(DESTDIR)$(PREFIX)/lib/libclh2of.so

uninstall: all
	rm -f $(DESTDIR)$(PREFIX)/include/clh2of.h $(DESTDIR)$(PREFIX)/bin/clhof-simple-dump.py $(DESTDIR)$(PREFIX)/bin/clh2of-pack $(DESTDIR)$(PREFIX)/bin/clh2of-simple-add $(DESTDIR)$(PREFIX)/bin/clh2of-simple-tabulate $(DESTDIR)$(PREFIX)/bin/clh2of-tabulate $(DESTDIR)$(PREFIX)/bin/clh2of-unpack $(DESTDIR)$(PREFIX)/lib/libclh2of.so

.PHONY: all clean env install uninstall

dist/bin/clh2of-simple-add: clh2of-simple-add.o
	mkdir -p dist/bin
	$(CC) -o $@ clh2of-simple-add.o

dist/bin/clh2of-simple-tabulate: \
    dist/lib/libclh2of.so \
    dist/tmp/clh2of-simple-tabulate.o
	mkdir -p dist/bin
	$(CC) -o $@ \
	    dist/tmp/clh2of-simple-tabulate.o \
	    -Ldist/lib -lclh2of -lsqlite3

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

dist/tmp/clh2of-simple-tabulate.o: \
    clh2of-simple-tabulate.c \
    dist/include/clh2of.h \
    utils.inl
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c \
	    clh2of-simple-tabulate.c

dist/tmp/clh2of-tabulate.o: \
    clh2of-tabulate.c \
    dist/include/clh2of.h \
    utils.inl
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c \
	    clh2of-tabulate.c

dist/tmp/clh2of.o: \
    clh2of.cc \
    dist/include/clh2of.h \
    dist/tmp/lpp.ok \
    dist/tmp/openfci/src/manybody/linalg.hpp \
    dist/tmp/openfci/src/manybody/simple_linalg.hpp \
    dist/tmp/openfci/src/manybody/simple_matrix.hpp \
    dist/tmp/openfci/src/manybody/simple_vector.hpp \
    dist/tmp/openfci/src/manybody/sparse.hpp \
    dist/tmp/openfci/src/quantumdot/QdotInteraction.hpp \
    dist/tmp/openfci/src/quantumdot/RadialPotential.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Idist/tmp/lpp -o $@ -c \
	    -Idist/tmp/openfci/src \
	    clh2of.cc

dist/include/clh2of.h: clh2of.h
	mkdir -p dist/include
	cp clh2of.h $@

dist/tmp/openfci/src/manybody/gauss_tools.o: \
    dist/tmp/lpp.ok \
    dist/tmp/openfci.ok
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Idist/tmp/lpp -o $@ -c \
	    dist/tmp/openfci/src/manybody/gauss_tools.cc

dist/tmp/openfci/src/quantumdot/QdotInteraction.o: \
    dist/tmp/lpp.ok \
    dist/tmp/openfci/src/manybody/gauss_tools.hpp \
    dist/tmp/openfci/src/manybody/linalg.hpp \
    dist/tmp/openfci/src/manybody/simple_linalg.hpp \
    dist/tmp/openfci/src/manybody/simple_matrix.hpp \
    dist/tmp/openfci/src/manybody/simple_vector.hpp \
    dist/tmp/openfci/src/manybody/sparse.hpp \
    dist/tmp/openfci/src/manybody/tools.hpp \
    dist/tmp/openfci/src/quantumdot/QdotInteraction.cc \
    dist/tmp/openfci/src/quantumdot/QdotInteraction.hpp \
    dist/tmp/openfci/src/quantumdot/RadialPotential.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Idist/tmp/lpp -o $@ -c \
	    dist/tmp/openfci/src/quantumdot/QdotInteraction.cc

dist/tmp/openfci/src/quantumdot/RadialPotential.o: \
    dist/tmp/lpp.ok \
    dist/tmp/openfci/src/manybody/gauss_tools.hpp \
    dist/tmp/openfci/src/manybody/linalg.hpp \
    dist/tmp/openfci/src/manybody/simple_linalg.hpp \
    dist/tmp/openfci/src/manybody/simple_matrix.hpp \
    dist/tmp/openfci/src/manybody/simple_vector.hpp \
    dist/tmp/openfci/src/manybody/tools.hpp \
    dist/tmp/openfci/src/quantumdot/RadialPotential.cc \
    dist/tmp/openfci/src/quantumdot/RadialPotential.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Idist/tmp/lpp -o $@ -c \
	    dist/tmp/openfci/src/quantumdot/RadialPotential.cc

dist/tmp/openfci/src/manybody/gauss_tools.hpp \
dist/tmp/openfci/src/manybody/linalg.hpp \
dist/tmp/openfci/src/manybody/simple_linalg.hpp \
dist/tmp/openfci/src/manybody/simple_matrix.hpp \
dist/tmp/openfci/src/manybody/simple_vector.hpp \
dist/tmp/openfci/src/manybody/sparse.hpp \
dist/tmp/openfci/src/manybody/tools.hpp \
dist/tmp/openfci/src/quantumdot/QdotInteraction.cc \
dist/tmp/openfci/src/quantumdot/QdotInteraction.hpp \
dist/tmp/openfci/src/quantumdot/RadialPotential.cc \
dist/tmp/openfci/src/quantumdot/RadialPotential.hpp: dist/tmp/openfci.ok

dist/tmp/openfci.ok:
	. ./tools && get_openfci dist/tmp

dist/tmp/lpp.ok:
	. ./tools && get_lpp dist/tmp

.SUFFIXES: .c .o

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<
