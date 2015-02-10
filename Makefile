CC    ?= gcc
CFLAGS = -std=c99

COMMON_SRC  = $(wildcard src/*.c)
TOXDUMP_SRC = $(wildcard src/toxdump/*.c)
TOXFILE_SRC = $(wildcard src/toxfile/*.c)
TEST_SRC    = $(wildcard test/*.c) $(COMMON_SRC)

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
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(TOXDUMP_LIBS)

toxfile: $(COMMON_SRC) $(TOXFILE_SRC)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

check: $(TEST_SRC)
	$(CC) -o check $^ $(CFLAGS) $(LIBS) -lcheck
	./check
.PHONY: check

clean: distclean
distclean:
	rm -rf toxdump toxfile check Testing/
