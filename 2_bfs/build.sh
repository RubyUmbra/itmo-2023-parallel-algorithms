#!/usr/bin/env bash

cmake --build ./cmake-build-release -DCMAKE_CXX_COMPILER=/opt/opencilk/bin/clang++ -DCMAKE_BUILD_TYPE=Release --target bfs
