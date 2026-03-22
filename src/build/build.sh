#!/bin/bash

CC=gcc
CXX=g++
CFLAGS="-Wall -Wextra -O2 -Isrc/include $(pkg-config --cflags sdl2 lua)"
CXXFLAGS="-Wall -Wextra -O2 -Isrc/include $(pkg-config --cflags sdl2 lua)"
LIBS="$(pkg-config --libs sdl2 lua) -lm -lassetbundler -lpthread -ldl"

# Build Rust static lib
cd src/tools/assetbundler
cargo build --release
cd ../../..

LIBS="$LIBS -Lsrc/tools/assetbundler/target/release"

mkdir -p bin

$CC $CFLAGS -c src/main/main.c -o main.o
$CC $CFLAGS -c src/includec/render.c -o render.o

$CXX $CXXFLAGS -c src/tools/piss/piss.cpp -o piss.o

$CXX -o bin/polar *.o -Lsrc/tools/assetbundler/target/release $LIBS

rm *.o
