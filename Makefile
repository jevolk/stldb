
CC = g++
WFLAGS = -pedantic                             \
         -Wall                                 \
         -Wextra                               \
         -Wcomment                             \
         -Waddress                             \
         -Winit-self                           \
         -Wuninitialized                       \
         -Wunreachable-code                    \
         -Wvolatile-register-var               \
         -Wvariadic-macros                     \
         -Woverloaded-virtual                  \
         -Wpointer-arith                       \
         -Wlogical-op                          \
         -Wcast-align                          \
         -Wcast-qual                           \
         -Wstrict-aliasing=2                   \
         -Wstrict-overflow                     \
         -Wwrite-strings                       \
         -Wformat-y2k                          \
         -Wformat-security                     \
         -Wformat-nonliteral                   \
         -Wfloat-equal                         \
         -Wdisabled-optimization               \
         -Wno-missing-field-initializers       \
         -Wmissing-format-attribute            \
         -Wno-unused-parameter                 \
         -Wno-unused-label                     \
         -Wsuggest-attribute=format

CCFLAGS += -std=c++11 -Ileveldb/include
LDFLAGS = -Lleveldb/ -lleveldb
TARGETS = test


all:  $(TARGETS)


clean:
	rm -f *.o $(TARGETS)

test: test.o
	$(CC) -o $@ $(CCFLAGS) $(WFLAGS) $(LIBPATH) $^ $(LDFLAGS)


test.o: test.cpp stldb/*.h
	$(CC) -c -o $@ $(CCFLAGS) $(WFLAGS) $<
