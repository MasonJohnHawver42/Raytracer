CC = gcc
INCLUDE = -I./include
LIB = -lm

SOURCE = src/util/common.c \
		 src/util/lexer.c \
		 src/resources/image.c \
		 src/resources/scene_noise.c \
		 src/math/common.c \
		 src/math/vector.c \
		 src/math/noise.c \
		 src/core/array.c \
		 src/resources/scene.c \
		 src/math/geometry.c

VIEW ?= feh

bin/hw0 : hw0.c $(SOURCE)
	$(CC) hw0.c $(SOURCE) -o hw0 $(INCLUDE) $(LIB)
	mv hw0 bin/.

bin/hw1a : hw1a.c $(SOURCE)
	$(CC) hw1a.c $(SOURCE) -o hw1a $(INCLUDE) $(LIB)
	mv hw1a bin/.


hw0: bin/hw0
	./bin/hw0
	$(VIEW) out/hw0.ppm

hw1a: bin/hw1a
	./bin/hw1a assets/hw1a.in out/hw1a.ppm
	$(VIEW) out/hw1a.ppm

all: hw0 hw1a
	@echo "FINISHED"

clean: 
	rm -f bin/* out/*
	