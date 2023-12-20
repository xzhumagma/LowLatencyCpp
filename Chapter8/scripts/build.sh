#!/bin/bash

# TODO - point these to the correct binary locations on your system.
CMAKE=$(which cmake)
NINJA=$(which ninja)

# Specify the source and build directories
#SOURCE_DIR="/home/xpzhu/project/Building-Low-Latency-Applications-with-CPP/Chapter8"
SOURCE_DIR="/Users/xpzhu/Dev/LowLatencyCpp/Chapter8"
BUILD_DIR="${SOURCE_DIR}/cmake-build-release"
# Create the build directory if it doesn't exist
mkdir -p "$BUILD_DIR"
$CMAKE -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=$NINJA -G Ninja -S . -B "$BUILD_DIR"

$CMAKE --build "$BUILD_DIR" --target clean -j 4
$CMAKE --build "$BUILD_DIR" --target all -j 4

#create a debug build
DEBUG_DIR="${SOURCE_DIR}/cmake-build-debug"
mkdir -p "$DEBUG_DIR"
$CMAKE -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=$NINJA -G Ninja -S . -B "$DEBUG_DIR"

$CMAKE --build "$DEBUG_DIR" --target clean -j 4
$CMAKE --build "$DEBUG_DIR" --target all -j 4
