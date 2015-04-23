all: main.o
	g++ -w -o main main.o -lcrypto -lssl

main.o: main.cpp
	g++ -c  -lssl -lcrypto -w main.cpp
clean:
	rm -r main.dSYM 
	rm main.o
