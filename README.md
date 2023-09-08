
# Raytracer 

Author: Mason J Hawver

# Part 0



hw0.c holds the main function for assignment 0. It first loads a scene from assets/hw0.in. it Then loads an image from assets/initials.ppm. it Then creates an image using the scene parameters and perlin noise. And finally it mixes the image I loaded with the generated image, and saves the result to assets/hw0.ppm

# Code Structure

Header files are stores in ./include. Soruce files are stored in ./src. Assets like ppm files and scene parameter files are stored it ./assets. The compiled program is stored in ./bin. A Makefiile is used as the build system.

Math -> stores the basic math functionality

Resources/Util -> has the basic functionality for loading and reading a file including a basic lexer that is used to load both scenes and images.

Resources (Dep: Math, Resources/Util) -> stores the functionality to load and save images and scenes, and operations that can be performed on them 