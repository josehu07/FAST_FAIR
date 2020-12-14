
.PHONY: all clean
.DEFAULT_GOAL := all

LIBS=../papi/src/libpapi.a -lrt -lm -lpthread
INCLUDES=-I./include -I../papi/src/
CFLAGS=-O3 -std=c++11

TARGET=btree

all: main

main: src/test.cpp
	g++ $(CFLAGS) -o $(TARGET) src/test.cpp $(LIBS)

clean: 
	rm -f $(TARGET)
