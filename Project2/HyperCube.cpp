#include "HyperCube.h";

// creates a hypercube
HyperCube::HyperCube(list<mat4> hyperCubeMatrixList, vec3 mPosition, Shader* shader, vec3 color) {

	renderMode = GL_TRIANGLES;
	// gets origin
	this->shader = shader;

	// list of secondary cubes
	this->hyperCubeMatrixList = hyperCubeMatrixList;

	//default settings for cube base
	this->defaultPosition = mPosition;
	this->mPosition = mPosition;
	this->yCubeAngle = 0.0f;
	this->xCubeAngle = 0.0f;
	this->zCubeAngle = 0.0f;
	this->scaleFactor = 0.2f;

	this->color = color;
}

// draws the base cube, then all of the other cubes in the list
void HyperCube::draw(mat4 worldOrientation) {

	shader->setVec3("objectColor", color);

	mat4 renderMatrix;

	mat4 rotateMatrix = rotate(mat4(1.0f), radians(xCubeAngle), vec3(1.0, 0.0, 0.0)) *
		rotate(mat4(1.0f), radians(yCubeAngle), vec3(0.0,1.0,0.0)) *
		rotate(mat4(1.0f), radians(zCubeAngle), vec3(0.0,0.0,1.0));
	// matrix for base cube
	mat4 hyperCubeMatrix = translate(mat4(1.0f), vec3(mPosition))
		* rotateMatrix
		* scale(mat4(1.0f), vec3(scaleFactor, scaleFactor, scaleFactor));

	renderMatrix = hyperCubeMatrix;

	// draws base cube
	shader->setMat4("model", worldOrientation * renderMatrix);
	glDrawArrays(renderMode, 0, 36);

	// draws each cube in the list
	// each cube is linked to the base cube's matrix
	for (list<mat4>::iterator it = hyperCubeMatrixList.begin(); it != hyperCubeMatrixList.end(); it++) {
		renderMatrix = hyperCubeMatrix * (*it);
		shader->setMat4("model", worldOrientation * renderMatrix);
		glDrawArrays(renderMode, 0, 36);
	}
}

// translates the base cube's position
void HyperCube::moveCube(vec3 translation) {
	this->mPosition += translation;
}

// changes the base cube's scale
void HyperCube::changeCubeScaling(float scale) {
	this->scaleFactor += scale;
}

// rotates the base cube's angle
void HyperCube::changeYRotation(float degrees) {
	this->yCubeAngle += degrees;
}

void HyperCube::changeXRotation(float degrees) {
	this->xCubeAngle += degrees;
}

void HyperCube::changeZRotation(float degrees) {
	this->zCubeAngle += degrees;
}

// puts the base cube's position, rotation, and scale at default
void HyperCube::repositionCube() {
	this->mPosition = defaultPosition;
	scaleFactor = 0.2f;
	yCubeAngle = 0.0f;
	xCubeAngle = 0.0f;
	zCubeAngle = 0.0f;
}

// changes rendering mode for the cubes
void HyperCube::changeRenderingMode(int mode) {
	renderMode = mode;
}
