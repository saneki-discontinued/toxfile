CC    ?= gcc
CFLAGS = -std=c99

LIBSY_SRC   = $(wildcard lib/libsy/src/*.c)
COMMON_SRC  = $(wildcard src/*.c) $(LIBSY_SRC)
TOXDUMP_SRC = $(wildcard src/toxdump/*.c)
TOXFILE_SRC = $(wildcard src/toxfile/*.c)
TEST_SRC    = $(wildcard test/*.c) $(COMMON_SRC)

INCLUDES     = -Ilib/libsy/include
LIBS         = -ltoxcore
TOXDUMP_LIBS = -ljansson

# If running tests (make check), encryption support not needed
ifeq ($(MAKECMDGOALS),check)
	TOXFILE_NO_ENC = true
endif

# Unless TOXFILE_NO_ENC=true, assume building with encryption support
ifneq ($(TOXFILE_NO_ENC),true)
	LIBS += -ltoxencryptsave
else
	CFLAGS += -DTOXFILE_NO_ENC
endif

all: toxdump toxfile

toxdump: $(COMMON_SRC) $(TOXDUMP_SRC)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(INCLUDES) $(TOXDUMP_LIBS)

toxfile: $(COMMON_SRC) $(TOXFILE_SRC)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(INCLUDES)

check: $(TEST_SRC)
	$(CC) -o check $^ $(CFLAGS) $(LIBS) $(INCLUDES) -lcheck
	./check
.PHONY: check

clean: distclean
distclean:
	rm -rf toxdump toxfile check Testing/
