#pragma once
#include <iostream>
#include <list>
#include <map>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

enum class GameState {start, playing, end};

struct Character {
	unsigned int TextureID; // ID handle of the glyph texture
	glm::ivec2   Size;      // Size of glyph
	glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
};

class GameLogic {
public:
	GameLogic(GLFWwindow* window, float screenWidth, float screenHeight);
	int buildFreeText();
	void update();
	void timer();
	void startGame();
	void playGame();
	void endGame();
	void retryGame();
	void gainPoints(int amt);
	void nextLevel(int amt);
	bool isPlaying();
	void drawUI();
	void renderText(std::string text, float x, float y, float scale, glm::vec3 color);
private:
	GameState gameState;
	GLFWwindow* window;
	float width;
	float height;
	int points;
	float startFrameTime;
	float timeLimit;
	float timeLeft;
	int level;
	Shader* shader;
	unsigned int VAO, VBO;
	std::map<GLchar, Character> Characters;
};