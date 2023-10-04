CC = gcc
INCLUDE = -I../include

LIB = -lm
FLAGS = -O3

SOURCE = ../src/core/common.c \
		 ../src/core/lexer.c \
		 ../src/core/array.c \
		 ../src/math/common.c \
		 ../src/math/vector.c \
		 ../src/math/noise.c \
		 ../src/math/geometry.c \
		 ../src/resources/image.c \
		 ../src/resources/scene.c \
		 ../src/resources/scene_noise.c \
		 ../src/raytracer/bvh.c \
		 ../src/raytracer/raytracer.c

compare : compare.c $(SOURCE)
	$(CC) compare.c $(SOURCE) -o compare $(INCLUDE) $(LIB) $(FLAGS)

clean:
	rm -f ./out/*
	rm -f ./diff/*

hw1a : clean compare
	python3 unit.py ./hw1a/ ../bin/hw1a ./compare

hw1b : clean compare
	python3 unit.py ./hw1b/ ../bin/hw1a ./compare
