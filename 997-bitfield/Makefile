all: bitfield

bitfield: bitfield.o
	g++ -o $@ $<

%.o: %.cc
	g++ -O2 -W -Wall -g -std=gnu++11 -c -o $@ $<
