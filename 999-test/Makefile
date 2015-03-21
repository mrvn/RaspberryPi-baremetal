all: test

test: fb.o trig.o cube.o test.o
	g++ -std=gnu++11 -O2 -W -Wall -g -o $@ $+

%.o: %.cc
	g++ -std=gnu++11 -O2 -W -Wall -g -c -o $@ $<

trig.o: trig.cc fixed.h trig.h templates.h
cube.o: cube.cc cube.h fixed.h
test.o: test.cc fixed.h trig.h vector.h matrix.h cube.h fb.h
fb.o: fb.cc fb.h

clean:
	rm -f test *.o

