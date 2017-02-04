SRC_FILES = $(wildcard src/*.c)
BUILD_FILES = $(patsubst src/%.c, build/%.o, ${SRC_FILES})
LIBS = libnm
LFLAGS = $(shell pkg-config --libs ${LIBS})
CFLAGS = -g $(shell pkg-config --cflags ${LIBS})

all: build ${BUILD_FILES}
	gcc -o build/nmwifi-menu ${BUILD_FILES} ${LFLAGS} -lncurses
clean:
	-rm -rf build/
build/%.o: src/%.c
	gcc ${CFLAGS} -c -o $@ $^
build:
	mkdir build
