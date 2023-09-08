CC = gcc
INCLUDE = -I./include
LIB = -lm

SOURCE = src/resources/util/common.c src/resources/util/lexer.c src/resources/image.c src/resources/scene.c src/math/common.c src/math/vector.c src/math/noise.c

bin/hw0 : hw0.c
	$(CC) hw0.c $(SOURCE) -o hw0 $(INCLUDE) $(LIB)
	mv hw0 bin/.


clean:
	rm -f bin/* assets/hw0.ppm

assets/hw0.ppm: bin/hw0
	./bin/hw0

view: assets/hw0.ppm
	feh assets/hw0.ppm

	