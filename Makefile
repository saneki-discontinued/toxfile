LIBS        = -ljansson -ltoxcore
COMMON_SRC  = $(wildcard src/*.c)
TOXDUMP_SRC = $(wildcard src/toxdump/*.c)

all: toxdump

toxdump: $(COMMON_SRC) $(TOXDUMP_SRC)
	gcc -o $@ $^ $(LIBS)
