LIBS   = -ldl -ltoxcore
SRC=$(wildcard *.c)

all: toxdump

toxdump: $(SRC)
	gcc -o $@ $^ $(LIBS)
