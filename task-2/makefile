all: main.o
	g++ -std=c++11 -w -o main main.o -lcrypto -lssl

main.o: main.cpp
	g++ -std=c++11 -c  -lssl -lcrypto -w main.cpp
clean:
	rm -r main.dSYM 
	rm main.o
