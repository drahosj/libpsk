#
# libpsk.so Makefile (Linux)
#
CC=c++
LINK=c++
OPTIONS=-g -fPIC -std=gnu++11

INCLUDES=-Iinc
CFLAGS=-Wall -Wextra -Werror -Wno-unused
COMPILE=$(CC) $(CFLAGS) $(INCLUDES)

PROGRAM=libpsk.so

SOURCES= calcimd.cpp \
pskdet.cpp \
pskmod.cpp \
pskcoresdr.cpp

HEADERS= calcimd.h \
pskdet.h \
pskdettbl.h \
psktables.h \
pskmod.h \
targetver.h \
pskcoresdr.h \
libpsk.h

OBJS= calcimd.o \
pskdet.o \
pskmod.o \
pskcoresdr.o \
libpsk.o

_HEADERS=$(addprefix inc/,$(HEADERS))
_SOURCES=$(addprefix inc/,$(HEADERS))

all: $(PROGRAM) $(_HEADERS) $(_SOURCES)

$(PROGRAM): $(OBJS)
	$(LINK) -shared -z noexecstack -o $(PROGRAM) $(OBJS) $(LIBS)

%.o: src/%.cpp
	$(COMPILE) $(OPTIONS) -c -o $@ $<

doc: inc/libpsk.h doxygen.conf
	doxygen doxygen.conf

clean:
	-rm -f *.o
	-rm -f $(PROGRAM)

test: $(PROGRAM)
	cd test && make run

dist-clean: clean
	-rm -rf doc
