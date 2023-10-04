CC = gcc
INCLUDE = -I./include

ifneq ("$(THREADED)", "")
	LIB = -lm -lpthread
	FLAGS = -O3 -DTHREAD_IT
	DECERATOR = _t
else
	LIB = -lm
	FLAGS = -O3
	DECERATOR = _n
endif

SOURCE = src/core/common.c \
		 src/core/lexer.c \
		 src/core/array.c \
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

HW_VERSION ?= hw1b
HW_NAME = $(HW_VERSION)$(DECERATOR)

raytracer1b : ./bin/$(HW_NAME)
	cp ./bin/$(HW_NAME) ./raytracerlb

# Make command for hw

./bin/$(HW_NAME) : $(HW_VERSION).c $(SOURCE)
	$(CC) $(HW_VERSION).c $(SOURCE) -o ./bin/$(HW_NAME) $(INCLUDE) $(LIB) $(FLAGS)


# Basic Phony targets

all: raytracer1b

clean: 
	rm -f ./raytracer1b

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

balls: bin/hw1b_t
	./bin/hw1b_t assets/scenes/balls.in assets/renders/balls.ppm
	$(VIEW) assets/renders/balls.ppm

views: hw0 hw1a hw1b balls

	