all: readwrite

readwrite: readwrite.o
	g++ readwrite.o -o readwrite

readwrite.o: readwrite.cpp
	g++ readwrite.cpp -c

clean: 
	rm *.o readwrite
