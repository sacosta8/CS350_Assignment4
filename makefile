all: readwrite

readwrite: readwrite.cpp
	g++ readwrite.cpp -g -o readwrite -lpthread


clean: 
	rm *.o readwrite
