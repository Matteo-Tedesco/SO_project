#!/bin/bash
make
wait
make main.hex
wait
cd build
./client