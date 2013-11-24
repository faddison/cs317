CC=gcc
CFLAGS=-Wall -g # -std=c99
LDFLAGS=
LDLIBS=

all: phreak1 phreak2 phreak3
phreak1: main1.o phreak.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)
phreak2: main2.o phreak.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)
phreak3: main3.o phreak.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

main1.o: main.c phreak.h
	$(CC) -DPART=1 $(CFLAGS) -c -o $@ $<
main2.o: main.c phreak.h
	$(CC) -DPART=2 $(CFLAGS) -c -o $@ $<
main3.o: main.c phreak.h
	$(CC) -DPART=3 $(CFLAGS) -c -o $@ $<
phreak.o: phreak.c phreak.h

clean:
	-rm -rf phreak.o main1.o main2.o main3.o phreak1 phreak2 phreak3