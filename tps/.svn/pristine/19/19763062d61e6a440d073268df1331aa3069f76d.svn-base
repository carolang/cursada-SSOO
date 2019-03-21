CC = g++
CFLAGS = -g -Wall -Werror
LDFLAGS = -pthread

BINARIES = backend

all: $(BINARIES)

backend: Backend_multi.cpp Backend_multi.h Enviar_recibir.o Enviar_recibir.h Encabezado.h Casillero.h RWLock.h RWLock.cpp
	$(CC) $(LDFLAGS) $(CFLAGS) Backend_multi.cpp RWLock.cpp Enviar_recibir.o -o backend

Enviar_recibir.o: Enviar_recibir.cpp Enviar_recibir.h Encabezado.h
	$(CC) $(CFLAGS) -c Enviar_recibir.cpp

clean:
	rm -f *.o
	rm -f $(BINARIES)
