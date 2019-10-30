#!/bin/bash
g++ -g -shared -fPIC -I/usr/include/python2.7 -I../sab_parser bml.cc ../sab_parser/bml.c -lpython2.7 -lboost_python -o bml.so
