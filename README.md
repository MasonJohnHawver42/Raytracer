
# Raytracer 

Author: Mason John Hawver \
Class: CSCI 5607

# Gallery

in progress ...

<details>
  <summary> old submission imgs </summary>

![Distorted](./assets/gallery/hw1a_final2.png)
![Distorted](./assets/gallery/hw0_final.png)

</details>

# Compiling and Running

Compiling and Running on Linux-x86: (If not use the use the corresponding programs on your system)
```
cd <path to mason.hawver.hw1b>
make all
./raytracer1b <input file name> <optional: output file name>
``` 

Example Usage:
```
./raytracer1b ./assets/scenes/hw1b.in ./hw1b.ppm
feh ./hw1b.ppm #view image

./raytracer1b ./assets/scenes/hw1b.in 
feh ./assets/scenes/hw1b.ppm #view image
```

# Code Structure

 * ./include -> stores header files, the header files are commented so go there for additional documentation.
 * ./include/core -> headers for core data structures, io, and parsing
 * ./include/math -> headers for vectors, rays, spheres, perlin noise, and common math functions and structures
 * ./include/resources -> headers for image and scene loading
 * ./include/raytracer -> headers for ray casting and shading
 * ./src -> stores source files for each header file, it mirrors the structure of ./include
 * ./ -> stores main functions and Makefiles, README.md, and raytracer1b
 
 less important:

 * ./assets -> a bunch of files 
 * ./bin -> all programs generated so far

# HW1b Writeup

todo ...

## Old HW Writeups

<details>
  <summary>HW0</summary>
  
# HW0 writeup

hw0_final.ppm is the image I'm turning in for assignment 0. hw0_alt.ppm is another image my program produced using the alt settings commented in assets/hw0.in.

hw0.c holds the main function for assignment 0. It first loads a scene from assets/hw0.in. it Then loads an image from assets/initials.ppm. it Then creates an image using the scene parameters and perlin noise. And finally it mixes the image I loaded with the generated image, and saves the result to assets/hw0.ppm

</details>


<details>
  <summary>HW1a</summary>
  
# HW1a Writeup

1) How does the apparent rotation of the scene with respect to the viewpoint change with changes in the direction of the ‘up’ vector?

When the up vector's projection on the plane normal to the view direction is rotated the apparent rotation is also rotated with it linearly. This can be seen in image 1 and 2 when image 2 up vector is moved on the x axis to rotate the image.

![Distorted](./assets/hw1a_imgs/unrot.png)

params:
* eye      0.0 -10.0 -10.0
* viewdir  0.0 1.0 1.0
* updir    0.25 1.0 0.0
* vfov     60.0

![Distorted](./assets/hw1a_imgs/60fov.png)

params:
* eye      0.0 -10.0 -10.0
* viewdir  0.0 1.0 1.0
* updir    0.25 1.0 0.0
* vfov     60.0

2)  How do changes in the field of view settings affect the appearance of the scene in your rendered image?

Decresing the fov zooms in the image, this can be seen in the first to images when the fov is decreased from 90 in img 2 to 60 in image 1. Increasing the fov distorts the image this can be seen in the last image where I drastically increave the fov to 120 and move the eye closser so it can be seen clearer.

![Distorted](./assets/hw1a_imgs/60fov.png)

params:
* eye      0.0 -10.0 -10.0
* viewdir  0.0 1.0 1.0
* updir    0.25 1.0 0.0
* vfov     60.0


![Distorted](./assets/hw1a_imgs/90fov.png)

params:
* eye      0.0 -10.0 -10.0
* viewdir  0.0 1.0 1.0
* updir    0.25 1.0 0.0
* vfov     90.0


![Distorted](./assets/hw1a_imgs/distorted.png)

params:
* eye      0.0 -3.0 -3.0
* viewdir  0.0 1.0 1.0
* updir    0.25 1.0 0.0
* vfov     120.0

3) How can the viewing parameters (e.g. the camera location, field of view settings, …) be adjusted to achieve a less exaggerated vs more exaggerated amount of apparent perspective distortion in your image?

If you get really close to the objects and increase the fov it distorts the image and becomes more exagerated as you get closer and as the fov aproached 180, this can be seen in the images bellow.

![Distorted](./assets/hw1a_imgs/distorted.png)

params:
* eye      0.0 -3.0 -3.0
* viewdir  0.0 1.0 1.0
* updir    0.25 1.0 0.0
* vfov     120.0

![Distorted](./assets/hw1a_imgs/more_distorted.png)

params:
* eye      0.0 -2.0 -2.0
* viewdir  0.0 1.0 1.0
* updir    0.25 1.0 0.0
* vfov     150.0

![Distorted](./assets/hw1a_imgs/wtf_distorted.png)

params:
* eye      0.0 -1.0 -1.0 #inside the blue sphere and see the other intersected inside
* viewdir  0.0 1.0 1.0
* updir    0.25 1.0 0.0
* vfov     179.0
</details>