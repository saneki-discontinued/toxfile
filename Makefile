LIBS        = -ljansson -ltoxcore
CFLAGS      = -std=c99
COMMON_SRC  = $(wildcard src/*.c)
TOXDUMP_SRC = $(wildcard src/toxdump/*.c)
TOXFILE_SRC = $(wildcard src/toxfile/*.c)

all: toxdump toxfile

toxdump: $(COMMON_SRC) $(TOXDUMP_SRC)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

toxfile: $(COMMON_SRC) $(TOXFILE_SRC)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

clean: distclean
distclean:
	rm -f toxdump toxfile
