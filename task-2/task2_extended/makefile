all: main.o
	g++ -std=c++11 -w -o main main.o -lssl -lcrypto 
main.o: main.cpp
	g++ -std=c++11 -c -w main.cpp -lssl -lcrypto 
clean:
	rm -r main.dSYM
	rm main.o
