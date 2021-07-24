CC = g++
LIBS_LINUX = -L /usr/include -lpthread
LIBS = -Iinclude/ -lpthread -std=c++14

CLIENT_F = client/main.cpp
SERVER_F = server/main.cpp

all: client server

bruns: server runs

brunc: client runc

runs:
	sudo ./bin/server

runc:
	./bin/client

client: ${CLIENT_F}
	${CC} ${CLIENT_F} -o bin/$@ ${LIBS}

server: ${SERVER_F}
	${CC} ${SERVER_F} -o bin/$@ ${LIBS}

test: test.cpp
	${CC} test.cpp -o test.o
	./test.o
	rm test.o

clean:
	rm -rf bin/*

