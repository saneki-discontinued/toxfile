CC    ?= gcc
CFLAGS = -std=c99

COMMON_SRC  = $(wildcard src/*.c)
TOXDUMP_SRC = $(wildcard src/toxdump/*.c)
TOXFILE_SRC = $(wildcard src/toxfile/*.c)

LIBS         = -ltoxcore
TOXDUMP_LIBS = -ljansson

all: toxdump toxfile

toxdump: $(COMMON_SRC) $(TOXDUMP_SRC)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(TOXDUMP_LIBS)

toxfile: $(COMMON_SRC) $(TOXFILE_SRC)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean: distclean
distclean:
	rm -f toxdump toxfile
