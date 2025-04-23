COMPILER = clang++
LIB_OPTS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
INCLUDE_PATHS = -Iinclude/
OUT = -o out
CFILES = main.cpp
PLATFORM := $(shell uname)

ifeq ($(PLATFORM), Darwin)
	COMPILER = clang++
	LIB_OPTS = -Llib/darwin/ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -lraylib
	CXXFLAGS = -std=c++17
else
	CXXFLAGS = -std=c++17
endif

build:
	$(COMPILER) $(CXXFLAGS) $(INCLUDE_PATHS) $(CFILES) $(OUT) $(LIB_OPTS)

run:
	./out

clean:
	rm -rf ./out
