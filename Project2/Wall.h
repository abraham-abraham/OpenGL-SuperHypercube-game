#pragma once
#include <iostream>
#include <list>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>

#include "shader.h"

using namespace glm;
using namespace std;

class Wall {
public:
	Wall(list<mat4> wallList, vec3 mPosition, Shader* shader, vec3 color);
	void draw(mat4 worldOrientation);

private:
	vec3 mPosition;
	Shader* shader;
	list<mat4> wallList;
	vec3 color;
};