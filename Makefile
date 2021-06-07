CC = g++
LIBS = -L /usr/include -lpthread

CLIENT_F = client/main.cpp
SERVER_F = server/main.cpp

all: client server

bruns: server runs

brunc: client runc

runs:
	sudo ./bin/server.app

runc:
	./bin/client.app

client: ${CLIENT_F}
	${CC} ${CLIENT_F} -o bin/$@.app ${LIBS}

server: ${SERVER_F}
	${CC} ${SERVER_F} -o bin/$@.app ${LIBS}

test: test.cpp
	${CC} test.cpp -o test.o
	./test.o
	rm test.o

clean:
	rm -rf bin/*

