CC=gcc
CXX=g++

CFLAGS=-Wall -O2
CXXFLAGS=$(CFLAGS) -std=c++11
LDFLAGS=-s

all: 1 2 compress decompress

1: huffman.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

2: demo.o huffman.o heapq.o
	$(CC) $(LDFLAGS) -o $@ demo.o huffman.o heapq.o

compress: compress.o bitio.o huffman.o heapq.o
	$(CC) $(LDFLAGS) -o $@ compress.o bitio.o huffman.o heapq.o

decompress: decompress.o bitio.o
	$(CC) $(LDFLAGS) -o $@ decompress.o bitio.o

clean:
	rm -f *.o

distclean:
	rm 1 2 compress decompress

.PHONY: all clean

huffman.o: huffman.c huffman.h heapq.h
heapq.o: heapq.c heapq.h
demo.o: demo.c huffman.h
compress.o: compress.c huffman.h bitio.h
decompress.o: decompress.c bitio.h
bitio.o: bitio.c bitio.h
