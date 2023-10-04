#!/bin/bash
make clean
wait
make
wait
make main.hex
wait
cd build
./client