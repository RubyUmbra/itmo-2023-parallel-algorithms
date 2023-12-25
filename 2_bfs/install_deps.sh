#!/usr/bin/env bash

OPENCILK_PATH=/opt/opencilk/bin
if [ "$1" ]; then
    OPENCILK_PATH="$1"
fi

echo "Using: ${OPENCILK_PATH}"

mkdir lib && cd lib
git clone https://github.com/deepsea-inria/pctl.git
git clone https://github.com/deepsea-inria/cmdline.git
git clone https://github.com/deepsea-inria/chunkedseq.git

CC=${OPENCILK_PATH}/clang
CXX=${OPENCILK_PATH}/clang++

mkdir cilkrts
git clone https://github.com/OpenCilk/cilkrts.git cilkrts-src && cd cilkrts-src
mkdir ./build && cd ./build
cmake -DCMAKE_C_COMPILER=${CC} -DCMAKE_CXX_COMPILER=${CXX} -DCMAKE_INSTALL_PREFIX=../../cilkrts ..
make && make install
cd ../.. && rm -rf cilkrts-src
