#!/bin/bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=OFF
cmake --build build

