all: main.o
	g++ -lssl -lcrypto -w -o main main.o

main.o: main.cpp
	g++ -c  -lssl -lcrypto -w main.cpp
clean:
	rm -r main.dSYM 
	rm main.o
