#!/bin/bash

mkdir -p ../

rm -fr ../build/libmydll.so

g++ -std=c++11 -fPIC -shared -o ../build/libmydll.so mydll.cpp

