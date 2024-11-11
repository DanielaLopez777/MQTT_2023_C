# Makefile para compilar Cliente.c y Servidor.c

CC=gcc

all: Cliente Servidor

Cliente: Cliente.c
	$(CC) ClientAction.c Cliente.c -o Cliente -lpthread

Servidor: Servidor.c
	$(CC) RcvCast.c Servidor.c -o Servidor -lpthread

clean:
	rm -f Cliente Servidor
