CC = g++
CFLAGS = -pthread -g -Wall


all :
	$(CC) $(CFLAGS) -o a.out MtGALogParser.cpp pch.cpp pch.h

test :
	$(CC) $(CFLAGS) -o test.out test.cpp

clean :
	rm *.out