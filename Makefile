CXXFLAGS := -O3 \
	    -Wall -Wextra -pedantic \
	    -Winit-self \
	    -Wshadow \
	    -Wformat=2 \
	    -Wmissing-declarations \
	    -Wstrict-overflow=5 \
	    -Wcast-align \
	    -Wcast-qual \
	    -Wconversion \
	    -Wunused-macros \
	    -Wwrite-strings \
	    -DNDEBUG \
	    -D_POSIX_C_SOURCE=200809L \
	    ${CXXFLAGS}

ifeq (${CXX}, g++)
	CXXFLAGS += -Wno-missing-field-initializers
endif

ifeq (${CXX}, clang++)
	CXXFLAGS += -Wimplicit-fallthrough
endif

CXX := ${CXX} -std=c++11

all: ini
ini: test.o ini.o
	${CXX} ${CXXFLAGS} $^ -o $@

.cpp.o:
	${CXX} ${CXXFLAGS} $^ -o $@

ini.cc: ini.rl
	ragel -C $< -o $@

clean:
	rm ini *.o

.PHONY: clean
