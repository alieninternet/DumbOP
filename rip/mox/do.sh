#!/bin/sh
c++ -Wall -c main.cc
c++ ../../src/string.o main.o -o main
