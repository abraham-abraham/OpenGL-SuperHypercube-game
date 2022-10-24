#include "Wall.h";

// Create wall object
Wall::Wall(list<mat4> wallList, vec3 mPosition, Shader* shader, vec3 color) {

	this->shader = shader;
	// cubes in the wall list
	this->wallList = wallList;
	// position in world
	this->mPosition = mPosition;

	this->color = color;
}

// draws wall object
void Wall::draw(mat4 worldOrientation) {

	shader->setVec3("objectColor", color);

	mat4 renderMatrix;

	// base matrix where the wall will be built around
	mat4 wallBaseMatrix = translate(mat4(1.0f), vec3(mPosition))
		* rotate(mat4(1.0f), radians(0.0f), vec3(1.0f, 0.0f, 0.0f))
		* scale(mat4(1.0f), vec3(0.2f, 0.2f, 0.2f));

	// each wall piece in the wall list is linked to the base matrix then drawn
	for (list<mat4>::iterator it = wallList.begin(); it != wallList.end(); it++) {
		renderMatrix = wallBaseMatrix * (*it);
		shader->setMat4("model", worldOrientation * renderMatrix);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}