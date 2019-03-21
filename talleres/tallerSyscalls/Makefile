# $@ significa el target de la operación
# $^ significa los archivos de los cuales depende el target

all: justiciero

justiciero: justiciero.o
	g++ -g -Wall -pedantic -o $@ $^

justiciero.o: justiciero.c
	g++ -g -Wall -pedantic -c -o $@ $^

clean:
	rm -rf justiciero.o
	rm -rf justiciero
