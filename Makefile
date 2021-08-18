CC = g++
LIBS_LINUX = -L /usr/include -lpthread
LIBS = -Iinclude -Llib -lpthread -std=c++14

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
	${CC} ${CLIENT_F} -o bin/$@ ${LIBS} -lSDL2

server: ${SERVER_F} server/server.cpp
	${CC} ${SERVER_F} -o bin/$@ ${LIBS}

test: test.cpp
	${CC} test.cpp -o test.o
	./test.o
	rm test.o

clean:
	rm -rf bin/*

