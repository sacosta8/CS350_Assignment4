all: readwrite

readwrite: readwrite.cpp
	g++ -Wall readwrite.cpp -g -o readwrite -lpthread -lrt


clean: 
	rm *.o readwrite

reader_clean:
	rm reader_*
