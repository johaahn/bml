#!/bin/bash
g++ -g -shared -fPIC -I/usr/include/python3.4 -I../sab_parser bml.cc ../sab_parser/bml.c -lpython3.4 -lboost_python -o bml.so
