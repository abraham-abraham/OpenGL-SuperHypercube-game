#pragma once
#include <iostream>
#include <list>
#include <string>

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

class HyperCube {
public:
	HyperCube(list<mat4> hyperCubeMatrixList, vec3 mPosition, Shader* shader, vec3 color);
	void draw(mat4 worldOrientation);
	void changeRenderingMode(int mode);
	void changeCubeScaling(float scale);
	void changeYRotation(float degrees);
	void changeXRotation(float degrees);
	void changeZRotation(float degrees);
	void moveCube(vec3 translation);
	void repositionCube();
private:
	vec3 mPosition;
	vec3 defaultPosition;
	vec3 color;
	list<mat4> hyperCubeMatrixList;
	int renderMode;
	float yCubeAngle;
	float xCubeAngle;
	float zCubeAngle;
	float scaleFactor;
	Shader* shader;
};