CC = gcc
INCLUDE = -I./include

SOURCE = src/core/common.c \
		 src/core/lexer.c \
		 src/core/array.c \
		 src/core/hashmap.c \
		 src/math/common.c \
		 src/math/vector.c \
		 src/math/noise.c \
		 src/math/geometry.c \
		 src/resources/image.c \
		 src/resources/scene.c \
		 src/resources/scene_noise.c \
		 src/raytracer/bvh.c \
		 src/raytracer/raytracer.c

VIEW ?= feh

HW_VERSIONS = hw1d hw1c hw1b hw1a hw0

targets = $(addprefix ./bin/, $(HW_VERSIONS))

targets_t = $(addsuffix _t, $(targets))

raytracer1d : ./bin/hw1d
	cp ./bin/hw1d ./raytracer1d

raytracer1c : ./bin/hw1c
	cp ./bin/hw1c ./raytracer1c

raytracer1b : ./bin/hw1b
	cp ./bin/hw1b ./raytracer1b

# Make command for hw

$(targets) : ./bin/% : %.c $(SOURCE)
	$(CC) $< $(SOURCE) -o $@ $(INCLUDE) -lm -g

$(targets_t) : ./bin/%_t : %.c $(SOURCE)
	$(CC) $< $(SOURCE) -o $@ $(INCLUDE) -lm -lpthread -O3 -DTHREAD_IT

# Basic Phony targets

all: raytracer1d

clean: 
	rm -f ./raytracer1b ./raytracer1c ./raytracer1d
	rm -f ./bin/*

# ignore: Phony Targets for quick and dirty image creation and viewing

hw0: bin/hw0
	./bin/hw0
	$(VIEW) assets/renders/hw0.ppm

hw1a: bin/hw1a
	./bin/hw1a assets/scenes/hw1a.in assets/renders/hw1a.ppm
	$(VIEW) assets/renders/hw1a.ppm

hw1b: bin/hw1b_t
	./bin/hw1b_t assets/scenes/hw1b.in assets/renders/hw1b.ppm
	$(VIEW) assets/renders/hw1b.ppm

hw1c: bin/hw1c_t
	./bin/hw1c_t assets/scenes/hw1c.in assets/renders/hw1c.ppm 
	$(VIEW) assets/renders/hw1c.ppm

hw1d: bin/hw1d_t
	./bin/hw1d_t assets/scenes/hw1d.in assets/renders/hw1d.ppm 
	$(VIEW) assets/renders/hw1d.ppm

balls: bin/hw1c_t
	./bin/hw1c_t assets/scenes/balls.in assets/renders/balls.ppm
	$(VIEW) assets/renders/balls.ppm

bunny: bin/hw1d_t
	./bin/hw1d_t assets/scenes/bunny2.in assets/renders/bunny.ppm
	$(VIEW) assets/renders/bunny.ppm

arm: bin/hw1c_t
	./bin/hw1c_t assets/scenes/arm.in assets/renders/arm.ppm
	$(VIEW) assets/renders/arm.ppm

dragon: bin/hw1d_t
	./bin/hw1d_t assets/scenes/dragon.in assets/renders/dragon.ppm
	$(VIEW) assets/renders/dragon.ppm


views: hw0 hw1a hw1b balls

	