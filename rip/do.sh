#!/bin/sh
c++ -Wall -c main.cc
c++ ../src/string.o ../src/stringtokenizer.o main.o -o main
