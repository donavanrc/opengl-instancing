# OpenGL - Instancing

![Demonstration](Screenshots/demo.png?raw=true "Title")

This program draws 3.5M cubes (42M triangles and 28M vertices) on the screen at an average of more than 60 fps.

The program was written procedurally and does not perform abstractions or any kind of functional organization, which means that this code is only for study and with the objective of showing in a simple way the necessary steps to work with instances in OpenGL.

## PC configuration:

- Processor Intel® Core™ i5-8400
- 8GB RAM (2400 GHz)
- Geforce GTX 1060 6GB
- Windows 10

## Project cloning

Start by cloning the repository with `git clone --recursive https://github.com/donavanrc/opengl-instancing.git`.

If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.

## Getting Started

For Visual Studio:

```
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
start opengl-instancing.sln
```

## Settings:

To configure the number of instances you want to render, just change the value of the `MAX_INSTANCES` constant inside the **main.cpp** file:

```cpp
const uint32_t MAX_INSTANCES = 3500000; // 3,5M Cubes (42M Triangles and 28M vertices)
```