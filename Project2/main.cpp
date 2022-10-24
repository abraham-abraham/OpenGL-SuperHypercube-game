#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <list>
#include <algorithm>
#include <vector>
#include <random>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Shader.h"
#include "game_logic.h"
//#include "headers/filesystem.h"

#include <IrrKlang/include/irrKlang.h>
//#include "C:\Users\Ibrah\OneDrive\Desktop\FinalProject-backup\FinalProject-backup\packages\IrrKlang\include\irrKlang.h"
//#include <irrKlang.h>
#include "OBJloaderV2.h"
#include "OBJloader.h"


#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glew.lib")

using namespace glm;
using namespace std;
using namespace irrklang;

ISoundEngine* SoundEngine = createIrrKlangDevice();

// Enable AMD
//extern "C"
//{
//	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
//}

// Enable Nvidea
//extern "C"
//{
//	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
//}

// Declared function "selectRotate" which is used for rotation of the selected letter
// Function is defined after main loop
void selectModelMovement(float& xPos, float& zPos, float& rotatex, float& rotatey, float& rotatez, float& scale);
bool initialize();
void input();
void setProjectionMatrix(int shaderProgram, mat4 projectionMatrix);
void setViewMatrix(int shaderProgram, mat4 viewMatrix);
void setWorldMatrix(int shaderProgram, mat4 worldMatrix);
void renderScene(const Shader& shader);
GLuint loadTexture(const char* filename);
int createTexturedCubeVertexArrayObject();
int createTexturedPlaneVertexArrayObject();
int createSkyboxVertexArrayObject();
unsigned int loadCubemap(vector<std::string> faces);
void loadLevel(Shader shader, int mesh[3][3][3]);
void renderQuad();
void moveCamera(vec3 Position, float horizantalAng, float verticalAng);
void resetCamera();
void moveModel(float xPos, float yPos, float zPos, float scale, float rotationX, float rotationY, float rotationZ);
void resetModel();
void resetModelPos();
void randomizeModelRotation();
vec3 randomizeColourVec();
void randomizeColour();
GLuint setupModelVBO(string path, int& vertexCount);
void loadModel(Shader s);
int createWallCubeVertexArrayObject();

void loadModel2(Shader s);
// Sets up a model using an Element Buffer Object to refer to vertex data
GLuint setupModelEBO(string path, int& vertexCount);

GLuint redTextureID;
GLuint blueTextureID;
// projectile used for lasers
class Projectile
{
public:
	Projectile(vec3 position, vec3 velocity, Shader shaderProgram, float dotProd) : mPosition(position), mVelocity(velocity), mDot(dotProd)
	{
		
		//mWorldMatrixLocation = glGetUniformLocation(shaderProgram, "model");
	}

	void Update(float dt)
	{
		mPosition += mVelocity * dt;

	}

	void Draw(Shader s) {
		// this is a bit of a shortcut, since we have a single vbo, it is already bound
		// let's just set the world matrix in the vertex shader

		s.setInt("isTextured", 1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, blueTextureID);


		mat4 worldMatrix = translate(mat4(1.0f), mPosition) * rotate(mat4(1.0f), radians(mDot), vec3(1.0f))* scale(mat4(1.0f), vec3(1.0f, 0.2f, 0.2f));
		
		s.setMat4("model", worldMatrix);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		s.setInt("isTextured", 0);

	}

private:
	GLuint mWorldMatrixLocation;
	vec3 mPosition;
	vec3 mVelocity;
	float mDot; 
};



#pragma region SETTINGS

GameLogic* gameLogic;

const GLuint WIDTH = 1920, HEIGHT = 1080;

float baseCubeScale = 1.0;

float modelZposition = 10.f;


int level = 1;
float wallZspace = -60.0f;

float gameSpeed = 0.04f;
//0.0f;
//float fastGameSpeed = 0.1f;


float selectMovement = 1.0f;


#pragma region Skybox
int skychoice = 0;

std::vector<string> skyVec{ "skybox0", "skybox1", "skybox2", "skybox3" };

#pragma endregion

float R = 0.6627f;
float G = 0.1627f;
float B = 0.9627f;

// Rendering parameters
#pragma region RENDER

// Light settings
glm::vec3 lightPos(-5.0f, 15.0f, 25.0f);

vec3 initialAmbient = vec3(1.0f);
vec3 initialDiffuse = vec3(1.0f);
vec3 initialSpecular = vec3(1.0f);
float initialShine = 32.0f;

GLuint isTextured = 1;
GLuint isShadow = 1;
GLuint isMaterial;

bool toggle = true;
bool shadowToggle = true;

#pragma endregion


// Camera parameters
#pragma region CAMERA

vec3 defaultCameraPosition(0.0f, 7.0f, 23.0f);

vec3 cameraFront(0.0f, 0.0f, -1.0f);
vec3 cameraUp(0.0f, 1.0f, 0.0f);


float defaultCameraHorizontalAngle = 90.0f;
float defaultCameraVerticalAngle = -10.0f;
const float defaultCameraAngularSpeed = 5.0f;


float fieldOfView = 45.0f;
float cameraZoom = 0.0f;

float cameraSpeed = 6.0f;
float cameraFastSpeed = 2 * cameraSpeed;

bool cameraMouseLocked = true;
bool fastCam;
float currentCameraSpeed;

int selectedCamera = 1;


int shootTime = 0.0f; 



mat4 viewMatrix;

//mat4 wallPositionMat;


vec3 cameraPosition = defaultCameraPosition;
float cameraHorizontalAngle = defaultCameraHorizontalAngle;
float cameraVerticalAngle = defaultCameraVerticalAngle;
float cameraAngularSpeed = defaultCameraAngularSpeed;



#pragma endregion

#pragma endregion

#pragma region GLOBAL VARIABLES
GLFWwindow* window = NULL;
//textures IDs
GLuint brickTextureID;
GLuint cementTextureID;
GLuint floorTextureID;
GLuint wallTextureID;
GLuint containerTextureID;
GLuint woodTextureID;
GLuint space2TextureID;
GLuint stonewallTextureID;
GLuint yellowWallTextureID;
GLuint orangeWallTextureID;
GLuint marbleTextureID;
GLuint comfTextureID;
GLuint metalTextureID;


//ufo textures

GLuint diffuseTextureID;
GLuint diffuseGlowTextureID;

GLuint diffuseTextureID2;
GLuint diffuseGlowTextureID2;

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
unsigned int texturedCubeVAO;
unsigned int wallCubeVAO;

unsigned int quadVAO = 0;
unsigned int quadVBO = 0;

unsigned int planeVAO = 0;
unsigned int planeVBO = 0;


unsigned int skyboxVAO = 0;
unsigned int skyboxVBO = 0;

// unsigned int skyboxVAO, skyboxVBO;

float alpha = 0.0f;
double mousePosX, mousePosY;
float theta;
float phi; // follow y axix
float dx;
float dy;


float lastFrameTime;
int lastMouseLeftState;
double lastMousePosX, lastMousePosY;



// for keyboard
double keyX{ 0.0 }, keyY{ 0.0 };
double lastKeyX{ 0.0 }, lastKeyY{ 0.0 };
float dxKey = keyX - lastKeyX;
float dyKey = keyY - lastKeyY;

float lastXState = GLFW_RELEASE;
float lastWState = GLFW_RELEASE;
float lastMState = GLFW_RELEASE;
float lastBState = GLFW_RELEASE;
float lastNUM8State = GLFW_RELEASE;
float lastNUM2State = GLFW_RELEASE;
float lastNUM4State = GLFW_RELEASE;
float lastNUM6State = GLFW_RELEASE;
float lastNUM1State = GLFW_RELEASE;
float lastNUM9State = GLFW_RELEASE;

float lastls1State = GLFW_RELEASE;
float lastls2State = GLFW_RELEASE;

float lastRstate = GLFW_RELEASE;

float lastOstate = GLFW_RELEASE;


float dt;
vec3 cameraSideVector;

// WorldRotate: Base Matrix for all others (used in world rotation)
float worldRotateX = 0.0f;
float worldRotateY = 0.0f;
float worldRotateNX = 0.0f;
float worldRotateNY = 0.0f;

// world orientation
mat4 worldRotate;


// shadow variables
const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
unsigned int depthMapFBO;
unsigned int depthMap;


// the skybox faces textures
//std::vector<std::string> faces;
//std::vector<std::string> facesCube;


// load model 
int vertexCount;
//string s1 = "../project2/heracles.obj";

//string s1 = "../project2/Low_poly_UFO.obj";

string s2 = "../project2/resources/Models/Low_poly_UFO.obj";

string s1 = "../project2/resources/Models/Low_poly_UFO.obj";

GLuint VAOT;
GLuint VAOT2;



unsigned int cubemapTexture;
unsigned int cubemapTexture0;
unsigned int cubemapTexture1;
unsigned int cubemapTexture2;
unsigned int cubemapTexture3;


std::vector<unsigned int> cubemapTextureVector{ cubemapTexture0, cubemapTexture1, cubemapTexture2, cubemapTexture3 };


// list of levels
std::vector<int> levelsVec{2,3,4,5,6,7,8,9,10,11};

int levelIndex = 0;

#pragma	endregion


#pragma region MODEL CTRL VARIABLES

float s = 1.0f;
float x = 0.0f;
float y = 0.0f;
float z = 0.0f;
float rx = 0.0f;
float ry = 0.0f;
float rz = 0.0f;

// new vars
bool keyLeft = false;
bool keyRight = false;
bool keyUp = false;
bool keyDown = false;
bool keyPgUp = false;
bool keyPgDown = false;
mat4 rMatrix = mat4(1.0f); 
float rAngle = 0.0f; 
float angleRotateSpeed = 150.0f;


#pragma endregion

#pragma region Meshes

int  mesh1[3][3][3] = {
	// far face,  mid face ,  near face

  //   back row      middle row     front row
	 {{0, 0, 0}, {1, 0, 1}, {0, 1, 1}}, // bottom plane
	 { {0, 0, 0}, {0, 0, 0}, {1, 0, 0} }, //middle plane
	 { {1, 0, 0}, {1, 0, 1}, {0, 0, 0} } }; // top plane

int  mesh2[3][3][3] = {

	// far face,  mid face ,  near face
	{ {1, 1, 1}, {1, 0, 1}, {1, 0, 1} },
	 { {1, 0, 0}, {1, 0, 0}, {0, 0, 0} }, 
	 { {1, 1, 0}, {0, 0, 0}, {0, 0, 0} } }; 


int  mesh3[3][3][3] = {

	// far face,  mid face ,  near face
	{ {1, 0, 1}, {0, 0, 0}, {0, 0, 0} },
	 { {1, 1, 0}, {0, 0, 0}, {0, 1, 0} }, 
	 { {0, 0, 1}, {0, 0, 0}, {0, 0, 0} } }; 


int  mesh4[3][3][3] = {

	// far face,  mid face ,  near face
	 { {0, 0, 1}, {0, 0, 1}, {0, 0, 1} },
	 { {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },
	 { {0, 0, 0}, {0, 0, 0}, {0, 0, 0} } };

int  mesh5[3][3][3] = {

	// far face,  mid face ,  near face
	{ {0, 0, 1}, {0, 0, 1}, {0, 0, 0} }, // bottom plane
	 { {0, 0, 0}, {0, 0, 0}, {0, 0, 0} }, // middle
	 { {0, 0, 0}, {1, 0, 1}, {1, 0, 0} } }; // top 

int  mesh6[3][3][3] = {
	// far face,  mid face ,  near face
	 { {1, 0, 1}, {1, 0, 0}, {1, 1, 0} },
	 { {1, 0, 0}, {1, 0, 0}, {0, 0, 0} },
	 { {1, 0, 0}, {1, 0, 0}, {1, 0, 0} } };

int  mesh7[3][3][3] = {
	// far face,  mid face ,  near face
	{ {1, 0, 0}, {1, 1, 0}, {1, 0, 0} },// bottom plane
	 { {0, 0, 0}, {0, 0, 0}, {0, 0, 0} }, // middle
	 { {1, 0, 0}, {1, 0, 0}, {0, 0, 0} } }; // top 

int  mesh8[3][3][3] = {
	// far face,  mid face ,  near face
	//   back row      middle row     front row
	{ {1, 0, 0}, {0, 0, 0}, {0, 0, 0} },// bottom plane
	 { {1, 0, 0}, {1, 0, 0}, {0, 0, 0} }, // middle
	 { {1, 1, 0}, {1, 0, 0}, {1, 1, 1} } }; // top

int  mesh9[3][3][3] = {
	// far face,  mid face ,  near face
	 { {0, 0, 1}, {0, 0, 1}, {0, 0, 1} },
	 { {0, 0, 1}, {0, 0, 1}, {0, 0, 0} },
	 { {0, 0, 1}, {1, 0, 0}, {1, 1, 0} } };

int  mesh10[3][3][3] = {
	// far face,  mid face ,  near face
	{ {0, 0, 0}, {0, 0, 0}, {0, 0, 0} },// bottom plane
	 { {1, 0, 1}, {1, 0, 1}, {0, 0, 0} },// middle
	 { {1, 1, 0}, {0, 0, 0}, {0, 0, 0} } };

int  mesh11[3][3][3] = {
	// far face,  mid face ,  near face
	 { {0, 1, 1}, {0, 0, 0}, {0, 0, 1} },
	 { {1, 0, 0}, {1, 0, 0}, {0, 0, 0} },
	 { {0, 0, 0}, {0, 0, 0}, {0, 0, 0} } };


#pragma endregion

//struct mesh
//{
//	int  meshes[3][3][3];
//	mesh(int  _meshes[3][3][3]) : meshes[3][3][3] (_meshes[3][3][3]){}
//}



// Textured Cube structure
struct TexturedColoredVertex
{
	vec3 position;
	vec3 normal;
	vec2 uv;
	
	TexturedColoredVertex(vec3 _position, vec3 _normal, vec2 _uv)

		: position(_position), normal(_normal), uv(_uv) {}


};

// Textured Cube model array
const TexturedColoredVertex texturedCubeVertexArray[] = {
							// position,                normal                  UV                     
TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f),  vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),      //left -
TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f),  vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.5f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.5f, 0.5f)),

TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.5f, 0.5f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.5f, 0.0f)),

TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.5f, 0.5f)),       // far - 
TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.5f)),

TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.5f, 0.5f)),
TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.5f, 0.0f)),
TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),

TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.5f, 0.5f)),        // bottom -
TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.5f, 0.0f)),

TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f,-1.0f, 0.0f), vec2(0.5f, 0.5f)),
TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f,-1.0f, 0.0f), vec2(0.0f, 0.5f)),
TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f,-1.0f, 0.0f), vec2(0.0f, 0.0f)),

TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f),  vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.5f)),    // near -
TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.5f, 0.0f)),

TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.5f, 0.5f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.5f)),
TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.5f, 0.0f)),

TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.5f, 0.5f)),      // right -
TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.5f, 0.0f)),

TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.5f, 0.5f)),
TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.5f)),

TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.5f, 0.5f)),    // top - 
TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.5f, 0.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),

TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.5f, 0.5f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.5f))
};


const TexturedColoredVertex WallCubeVertexArray[] = {
	// position,                normal                  UV                     
TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f),  vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),      //left -
TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f),  vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),

TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),

TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f)),       // far - 
TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 1.0f)),

TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f)),
TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f)),
TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),

TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 1.0f)),        // bottom -
TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 0.0f)),

TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f,-1.0f, 0.0f), vec2(1.0f, 1.0f)),
TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f,-1.0f, 0.0f), vec2(0.0f, 1.0f)),
TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f,-1.0f, 0.0f), vec2(0.0f, 0.0f)),

TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f),  vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)),    // near -
TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),

TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)),
TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),

TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),      // right -
TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),

TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),

TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)),    // top - 
TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),

TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f))
};


// Textured Plan model array
float planeVertices[] = {
	// changed texcoords
	// positions            // normals         // texcoords
	
	-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,		// lower-right corner
	-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  10.0f,		// lower-right corner
	 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   1.0f,  0.0f,		// top-left corner  

	  25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   1.0f,  0.0f,		// lower-right corner  
	 -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  10.0f,		// left-upper corner
	  25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   1.0f,  10.0f		// upper-right corner
};

// Skybox array
float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

void renderScene(const Shader& shader)
{
	worldRotate =
		rotate(mat4(1.0f), radians(worldRotateX), vec3(1.0f, 0.0f, 0.0f))
		* rotate(mat4(1.0f), radians(worldRotateY), vec3(0.0f, 1.0f, 0.0f))
		* rotate(mat4(1.0f), radians(worldRotateNX), vec3(-1.0f, 0.0f, 0.0f))
		* rotate(mat4(1.0f), radians(worldRotateNY), vec3(0.0f, -1.0f, 0.0f));


	// check if scene should have shadows or not
	if (shadowToggle == true) {
		isShadow = 1;
	}
	else { isShadow = 0; }

	// check if scene should have texture or not
	if (toggle == true) {
		isTextured = 1;
	}
	else { isTextured = 0; }


	// setting texture/no texture
	shader.setInt("isTextured", isTextured);

	// setting shadow/no shadow
	shader.setInt("isShadow", isShadow);


	// Setting color
	// 
	// use shader.setVec3("objectColor", vec3( R f, G f, B f));
	// to set the colour of your objects because it needs to have 
	// a colour and the texture
	//	shader.setVec3("objectColor", vec3(0.8627f, 0.8627f, 0.8627f));
	// setting initial object colour to white
	shader.setVec3("objectColor", vec3(1.0f, 1.0f, 1.0f));


	// setting variables to have no material properties
	shader.setVec3("matAmbient", initialAmbient);
	shader.setVec3("matDiffuse", initialDiffuse);
	shader.setVec3("matSpecualr", initialSpecular);
	shader.setFloat("matShine", initialShine);


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);


	// Drawing the floor:
#pragma region DRAWING FLOOR	

	// Setting texture for the floor
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTextureID);

	// setting colour for floor

	shader.setVec3("objectColor", vec3(0.478431f, 0.64706f, 0.80196f));

	//float scalePlane = 2.0f;

	float scalePlane = 0.035f;

	//mat4 scalePlaneMat = scale(mat4(1.0f), vec3(scalePlane));

	mat4 scalePlaneMat = scale(mat4(1.0f), vec3(scalePlane, 1.0f, 1.0f));

	mat4 translateFloor = translate(mat4(1.0f), (vec3(0.0f , 3.0f , 8.0f)));


	mat4 plane = worldRotate * translateFloor * scalePlaneMat;

	shader.setMat4("model", plane);
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

#pragma	endregion

	// Binding the textured cube to render the scene
	glBindVertexArray(texturedCubeVAO);

	
	
	// Draw world cube section around grid
#pragma region WORLD CUBE

	/*glBindTexture(GL_TEXTURE_2D, space2TextureID);
	glActiveTexture(GL_TEXTURE0);


	mat4 cubeMatrix;
	cubeMatrix = translate(mat4(1.0f), vec3(0.0f, -0.01f, 0.0f)) * scale(mat4(1.0f), vec3(100.0f, 100.0f, 100.0f));
	cubeMatrix = worldRotate * cubeMatrix;

	shader.setMat4("model", cubeMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);*/

#pragma endregion



	// Setting texture
	//glBindTexture(GL_TEXTURE_2D, brickTextureID);
	//glBindVertexArray(texturedCubeVAO);
	//
	//to have a "shiny metal finish"
	// use:
	//shader.setVec3("matAmbient", vec3(x f, y f, z f));
	//shader.setVec3("matDiffuse", vec3(x f, y f, z f));
	//shader.setVec3("matSpecualr", vec3(x f, y f, z f));
	//shader.setFloat("matShine", f);
	//
	// Material values from: http://www.it.hiof.no/~borres/j3d/explain/light/p-materials.html
	// or
	// http://devernay.free.fr/cours/opengl/materials.html
	// or there are some in the lecture slides
	//
	// Movement func
	//mat4 translateRotateMat = translate(mat4(1.0f), (vec3(5 * cos(alpha), 5.0f * sin(alpha), 0.0f)));
	//alpha += 0.01f;


	// stopping textures
	//shader.setInt("isTextured", 0);


	// Drawing cube around light source
#pragma region LIGHT CUBE

	// Set color
	shader.setVec3("objectColor", vec3(0.12156f, 0.52549f, 0.743137f));

	// light cube matrix
	mat4 lightCube = translate(mat4(1.0f), (lightPos + vec3(0.0f, 0.0f, 0.0f))) * scale(mat4(1.0f), vec3(2.0f));

	shader.setMat4("model", lightCube);
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 36);

#pragma endregion


#pragma region DRAWING AXES

	// Axis scale
	float axisScale = 1.0f;

	//Draw x-axis
	mat4 axisWorldMatrix = translate(mat4(1.0f), vec3(axisScale / 2, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(axisScale, 0.1f, 0.1f));

	shader.setVec3("objectColor", vec3(1.0f, 0.0f, 0.0f));
	shader.setMat4("model", axisWorldMatrix);
	//glDrawArrays(GL_TRIANGLES, 0, 36);

	//Draw y-axis
	axisWorldMatrix = translate(mat4(1.0f), vec3(0.0f, axisScale / 2, 0.0f)) * scale(mat4(1.0f), vec3(0.1f, axisScale, 0.1f));

	shader.setVec3("objectColor", vec3(0.0f, 1.0f, 0.0f));
	shader.setMat4("model", axisWorldMatrix);
	//glDrawArrays(GL_TRIANGLES, 0, 36);

	//Draw z-axis
	axisWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, axisScale / 2)) * scale(mat4(1.0f), vec3(0.1f, 0.1f, axisScale));

	shader.setVec3("objectColor", vec3(0.0f, 0.0f, 1.0f));
	shader.setMat4("model", axisWorldMatrix);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	glEnable(GL_DEPTH_TEST);

#pragma endregion


#pragma region Modeling

	shader.setVec3("objectColor", vec3(0.12156f, 0.52549f, 0.743137f));
	// setting back to having no material properties
	shader.setVec3("matAmbient", initialAmbient);
	shader.setVec3("matDiffuse", initialDiffuse);
	shader.setVec3("matSpecualr", initialSpecular);
	shader.setFloat("matShine", initialShine);

	shader.setInt("isMaterial", 0);



#pragma endregion

	// Rendering done
}



int main(int argc, char* argv[])
{
	// Initialize GLFW and OpenGL version
	glfwInit();

	// Initializing common variables
	initialize();

	// Gray background
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	shootTime++;



	texturedCubeVAO = createTexturedCubeVertexArrayObject();

	wallCubeVAO = createWallCubeVertexArrayObject();

	planeVAO = createTexturedPlaneVertexArrayObject();

	skyboxVAO = createSkyboxVertexArrayObject();

	//VAOT = setupModelVBO(s1, vertexCount);

	
	list<Projectile> projectileList;


	VAOT2 = setupModelVBO(s2, vertexCount);


	random_device randomSeed;
	auto randFunc = default_random_engine{ randomSeed() };
	shuffle(levelsVec.begin(), levelsVec.end(), randFunc);


	// For frame time
	lastFrameTime = glfwGetTime();


	glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

	// Other OpenGL states to set once
	// glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND); glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Compile and link shaders 
	// Linking shaders:
	Shader theTexturedShaderProgram("./shader/TexturedVertexShader.vs.glsl", "./shader/TexturedFragmentShader.fs.glsl");
	Shader shader("./shader/shadow_mapping.vs.glsl", "./shader/shadow_mapping.fs.glsl");
	Shader simpleDepthShader("./shader/shadow_mapping_depth.vs.glsl", "./shader/shadow_mapping_depth.fs.glsl");


	// debug shader
	//Shader debugDepthQuad("./shader/help shaders/debug_quad.vs.glsl", "./shader/help shaders/debug_quad_depth.fs.glsl");
	//
	//Shader shaderScene("./shader/cone/scene_vertex.glsl"
	//	, "./shader/cone/scene_fragment.glsl");
	//
	//Shader shaderShadow("./shader/cone/shadow_vertex.glsl"
	//	, "./shader/cone/shadow_fragment.glsl");

	// SkyBox



	Shader skyboxShader("./shader/skybox.vs.glsl", "./shader/skybox.fs.glsl");
	
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	for (int i = 0; i < 4; i++) {
		std::vector<std::string> faces =
		{
			("./resources/textures/skybox/" + skyVec.at(i % 4) + "/right.jpg"),
			("./resources/textures/skybox/" + skyVec.at(i % 4) + "/left.jpg"),
			("./resources/textures/skybox/" + skyVec.at(i % 4) + "/top.jpg"),
			("./resources/textures/skybox/" + skyVec.at(i % 4) + "/bottom.jpg"),
			("./resources/textures/skybox/" + skyVec.at(i % 4) + "/front.jpg"),
			("./resources/textures/skybox/" + skyVec.at(i % 4) + "/back.jpg")
		};

		//cubemapTexture = loadCubemap(faces);


		cubemapTextureVector.insert(cubemapTextureVector.begin(),loadCubemap(faces));
	}
	cubemapTexture = cubemapTextureVector.at(0);

#pragma region CONFIGURE DEPTH MAP
	// configure depth map FBO

	// -----------------------

	// generating a frame buffer to capture the shadow map
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion

	// 
	// --------------------

		/*debugDepthQuad.use();
		debugDepthQuad.setInt("depthMap", 0);*/



	gameLogic = new GameLogic(window, WIDTH, HEIGHT);

	SoundEngine->play2D("Fahrenheit.mp3", GL_TRUE);

	// Entering Main Loop
	while (!glfwWindowShouldClose(window))
	{
		

		
			

		// Each frame, reset color of each pixel to glClearColor
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(texturedCubeVAO);

		shader.use();
		shader.setInt("diffuseTexture", 0);
		shader.setInt("shadowMap", 1);

		shader.setInt("objTexture", 2);
		shader.setInt("obj2Texture", 3);


#pragma region SHADOW RENDER
		// 1. render depth of scene to texture (from light's perspective)
		// --------------------------------------------------------------
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		
		// decreased near plane from 20.1 to 10.1
		float near_plane = 10.1f, far_plane = 100.0f;
		lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, vec3(0.0f, 0.0f, -20.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		simpleDepthShader.use();
		simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		simpleDepthShader.setVec3("lightPos", lightPos);
		simpleDepthShader.setFloat("far_plane", far_plane);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		renderScene(simpleDepthShader);
		
		// Set projection matrix
		mat4 projectionMatrix = glm::perspective(
			fieldOfView + cameraZoom,            // field of view in degrees
			WIDTH * 1.0f / HEIGHT, // aspect ratio
			0.01f, 100.0f);   // near and far (near > 0)

		// set the view matrix
		viewMatrix = lookAt(cameraPosition, (cameraPosition + cameraFront), cameraUp);

		if (level == 1) {
			loadLevel(simpleDepthShader, mesh1);
		}
		else if (level == 2) {
			loadLevel(simpleDepthShader, mesh2);
		}
		else if (level == 3) {
			loadLevel(simpleDepthShader, mesh3);
		}
		else if (level == 4) {
			loadLevel(simpleDepthShader, mesh4);
		}
		else if (level == 5) {
			loadLevel(simpleDepthShader, mesh5);

		}
		else if (level == 6) {
			loadLevel(simpleDepthShader, mesh6);

		}
		else if (level == 7) {
			loadLevel(simpleDepthShader, mesh7);

		}
		else if (level == 8) {
			loadLevel(simpleDepthShader, mesh8);

		}
		else if (level == 9) {
			loadLevel(simpleDepthShader, mesh9);

		}
		else if (level == 10) {
			loadLevel(simpleDepthShader, mesh10);

		}
		else if (level == 11) {
			loadLevel(simpleDepthShader, mesh11);

		}
		else {
			gameLogic->endGame();
			level = 1;
			gameSpeed = 0.04f;
			wallZspace = -60.0f;
		}
		
		loadModel(simpleDepthShader);


		loadModel2(simpleDepthShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion

		//		shader.use();

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		// 2. render the scene as normal using the generated depth/shadow map
		// --------------------------------------------------------------
		// reset viewport
		glViewport(0, 0, width, height);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();



		shader.setMat4("view", viewMatrix);
		shader.setMat4("projection", projectionMatrix);

		// set light uniforms
		shader.setVec3("viewPos", cameraPosition);
		shader.setVec3("lightPos", lightPos);
		shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);



		input();

		renderScene(shader);

		for (list<Projectile>::iterator it = projectileList.begin(); it != projectileList.end(); ++it)
		{
			it->Update(dt);
			it->Draw(shader);
		}

		if (level == 1) {
			loadLevel(shader, mesh1);
		}
		else if (level == 2) {
			loadLevel(shader, mesh2);
		}
		else if (level == 3) {
			loadLevel(shader, mesh3);
		}
		else if (level == 4) {
			loadLevel(shader, mesh4);
		}
		else if (level == 5) {
			loadLevel(shader, mesh5);

		}
		else if (level == 6) {
			loadLevel(shader, mesh6);

		}
		else if (level == 7) {
			loadLevel(shader, mesh7);

		}
		else if (level == 8) {
			loadLevel(shader, mesh8);

		}
		else if (level == 9) {
			loadLevel(shader, mesh9);

		}
		else if (level == 10) {
			loadLevel(shader, mesh10);

		}
		else if (level == 11) {
			loadLevel(shader, mesh11);

		}
		else {
			gameLogic->endGame();
			level = 1;
			gameSpeed = 0.04f;
			wallZspace = -60.0f;
		}

		loadModel(shader);

		loadModel2(shader);


		// Frame time calculation
		dt = glfwGetTime() - lastFrameTime;
		lastFrameTime += dt;


		// Play Game
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			gameLogic->startGame();
		}
		// Test Score
		/*if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
			gameLogic.gainPoints(1);
		}*/
		// Retry
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			if (!gameLogic->isPlaying()) {
				SoundEngine->stopAllSounds();
				SoundEngine->play2D("Fahrenheit.mp3", GL_TRUE);
			}

			gameLogic->retryGame();
		}

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		mat4 rotateFlipMatrixY = rotate(mat4(1.0f), radians(270.0f), vec3(0.0f, 1.0f, 0.0f));
		mat4 rotateFlipMatrixX = rotate(mat4(1.0f), radians(180.0f), vec3(1.0f, 0.0f, 0.0f));
		mat4 view = glm::mat4(glm::mat3(viewMatrix)); // remove translation from the view matrix
		view = view * rotateFlipMatrixX * rotateFlipMatrixY;

		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projectionMatrix);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		gameLogic->update();



		if (gameLogic->isPlaying()) {

			cameraZoom = 12.0f;
			cameraPosition.z = z + 25.0f;
			cameraPosition.y = 8.0f;

			// moving the model forward

			if (z >= wallZspace / 2 - 5)
			{
				// Speed up
				if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
					z -= gameSpeed * 3;
				}
				else
					z -= gameSpeed;
			}
			else
			{
				resetModel();
				//level++;

				if (levelIndex > 9) {
					level = 0;
					levelIndex = 0;
				}

				else {
					level = levelsVec[levelIndex];

					levelIndex++;
				}

				rMatrix = mat4(1.0f);
				wallZspace += 1.0f;
				if(gameSpeed < 0.068f)
				gameSpeed += 0.003f;
				randomizeModelRotation();
				gameLogic->gainPoints(100);
				gameLogic->nextLevel(1);
				randomizeColour();
				SoundEngine->play2D("plasma.wav", GL_FALSE);
				cubemapTexture = cubemapTextureVector.at(level % 4);
				
			}

		}

		
		if (shootTime % 50 == 0) {
			float posX = (rand() % 50) - 25 ;
			float posY = (rand() % 50) - 25 ;

			float velx = (rand() % 20) - 10;
			float vely = (rand() % 20) - 10;

			const float projectileSpeed = 10.0f;

			float dotProd = dot(vec3(posX, posY, 40.0f), vec3(velx, vely, -20.0f));
			projectileList.push_back(Projectile(vec3(posX, posY, 40.0f), projectileSpeed * vec3(velx, vely, -20.0f), shader, dotProd));

		}

		// "NUM 8" rotate clockwise around the x-axis
		if (lastOstate == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		{
			resetModelPos();
		}
		lastOstate = glfwGetKey(window, GLFW_KEY_O);


	



		// End Frame
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}



// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}



void setProjectionMatrix(int shaderProgram, mat4 projectionMatrix)
{
	glUseProgram(shaderProgram);
	GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
}

void setViewMatrix(int shaderProgram, mat4 viewMatrix)
{
	glUseProgram(shaderProgram);
	GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
}

void setWorldMatrix(int shaderProgram, mat4 worldMatrix)
{
	glUseProgram(shaderProgram);
	GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
}

// Load texture
GLuint loadTexture(const char* filename)
{
	// Step1 Create and bind textures
	GLuint textureId = 0;
	glGenTextures(1, &textureId);
	assert(textureId != 0);
	glBindTexture(GL_TEXTURE_2D, textureId);

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	// Step2 Set filter parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

	// Step3 Load Textures with dimension data
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
		return 0;
	}

	// Step4 Upload the texture to the PU
	GLenum format = 0;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
		0, format, GL_UNSIGNED_BYTE, data);

	// Step5 Free resources
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureId;
}


// Creat the textured cube
int createTexturedCubeVertexArrayObject()
{
	// Create a vertex array
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	// Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texturedCubeVertexArray), texturedCubeVertexArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
		3,                   // size
		GL_FLOAT,            // type
		GL_FALSE,            // normalized?
		sizeof(TexturedColoredVertex), // stride - each vertex contain 2 vec3 (position, color)
		(void*)0             // array buffer offset
	);
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(TexturedColoredVertex),
		(void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
	);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2,                            // attribute 2 matches aUV in Vertex Shader
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(TexturedColoredVertex),
		(void*)(2 * sizeof(vec3))      // uv is offseted by 2 vec3 (comes after position and color)
	);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3,                            // attribute 3 matches aNormal in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(TexturedColoredVertex),
		(void*)(2 * sizeof(vec3) + sizeof(vec2))      // uv is offseted by 2 vec3 and one vec2 (comes after position and color and UV)
	);
	glEnableVertexAttribArray(3);


	return vertexArrayObject;
}

int createWallCubeVertexArrayObject()
{
	// Create a vertex array
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	// Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(WallCubeVertexArray), WallCubeVertexArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
		3,                   // size
		GL_FLOAT,            // type
		GL_FALSE,            // normalized?
		sizeof(TexturedColoredVertex), // stride - each vertex contain 2 vec3 (position, color)
		(void*)0             // array buffer offset
	);
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(TexturedColoredVertex),
		(void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
	);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2,                            // attribute 2 matches aUV in Vertex Shader
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(TexturedColoredVertex),
		(void*)(2 * sizeof(vec3))      // uv is offseted by 2 vec3 (comes after position and color)
	);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3,                            // attribute 3 matches aNormal in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(TexturedColoredVertex),
		(void*)(2 * sizeof(vec3) + sizeof(vec2))      // uv is offseted by 2 vec3 and one vec2 (comes after position and color and UV)
	);
	glEnableVertexAttribArray(3);


	return vertexArrayObject;
}

// Creat the textured plane
int createTexturedPlaneVertexArrayObject() {
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	// plane VAO

	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);

	glGenBuffers(1, &planeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);

	return planeVAO;
}

int createSkyboxVertexArrayObject() {

	// skybox VAO

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	return skyboxVAO;
}

// Common initialization
bool initialize() {

#if defined(PLATFORM_OSX)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
	// On windows, we set OpenGL version to 2.1, to support more hardware
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

	// Create Window and rendering context using GLFW, resolution is initially 1024x768,
	// but changes as the window is resized

	//glfwGetPrimaryMonitor()

	//glfwGetPrimaryMonitor()
	//glfwGetPrimaryMonitor()

	window = glfwCreateWindow(WIDTH, HEIGHT, "Comp371 - Project", NULL, NULL);

	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to create GLEW" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Load Textures
#if defined(PLATFORM_OSX)
	GLuint brickTextureID = loadTexture("Textures/brick.jpg");
	GLuint cementTextureID = loadTexture("Textures/cement.jpg");
#else
	redTextureID = loadTexture("./resources/textures/redlaser.png");
	blueTextureID = loadTexture("./resources/textures/bluelaser.jpg");
	
	brickTextureID = loadTexture("./resources/textures/brick.jpg");
	cementTextureID = loadTexture("./resources/textures/cement.jpg");
	floorTextureID = loadTexture("./resources/textures/spacestation.jpg");
	containerTextureID = loadTexture("./resources/textures/container.jpg");
	woodTextureID = loadTexture("./resources/textures/wood.png");
	//wallTextureID = loadTexture("wall.jpg");

	// more textures
	space2TextureID = loadTexture("./resources/textures/2space2048.jpg");
	stonewallTextureID = loadTexture("./resources/textures/stonewall.jpg");
	yellowWallTextureID = loadTexture("./resources/textures/yellowWall.jpg");
	orangeWallTextureID = loadTexture("./resources/textures/orangeWall.jpg");
	//greentileTextureID = loadTexture("./resources/textures/greentile.jpg");
	marbleTextureID = loadTexture("./resources/textures/marble.jpg");

	metalTextureID = loadTexture("./resources/textures/spacestation.jpg");
	wallTextureID = loadTexture("./resources/textures/wall.jpg");
	
	// ufo 
	diffuseTextureID = loadTexture("./ufo_diffuse.png");
	diffuseGlowTextureID = loadTexture("./ufo_diffuse_glow.png");

	// ufo2 
	diffuseTextureID2 = loadTexture("./resources/Models/ufo_diffuse.png");
	diffuseGlowTextureID2 = loadTexture("./resources/Models/ufo_diffuse_glow.png");

#endif
}


// call to move Models individually
void selectModelMovement(float& xPos, float& zPos, float& rotatex, float& rotatey, float& rotatez, float& scale) {

	// Translations:
#pragma region MODEL MOVEMENT
	// "KP_4 + Shift" move cube to the left
	if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS && ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)))
	{
		//xPos -= 0.06f;
	}

	// "KP_6 + Shift" move cube to the right
	if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS && ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)))
	{

		//	xPos += 0.06f;
	}

	// "KP_2 + Shift" continuous movement forward (towards +z axis)
	if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS && ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)))
	{
		zPos += 0.2f;
		// cameraPosition.z += 0.2f;
	}

	// "KP_8 + Shift" continuous movement forward (towards -z axis)
	if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS && ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))) // move olaf backwards
	{
		zPos -= 0.2f;
		// cameraPosition.z -= 0.2f;
	}


#pragma endregion

	// Rotations:
#pragma region Rotation Input

	// changed to down arrow
	// "NUM 8" rotate clockwise around the x-axis
	if (lastNUM8State == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS
	//if (lastNUM8State == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS
		&& !((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			|| (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
		&& !keyUp && !keyPgUp && !keyPgDown  && !keyLeft && !keyRight)
	{
		//rotatex -= 90.0f;
		keyDown = true;

	}
	//lastNUM8State = glfwGetKey(window, GLFW_KEY_KP_8);
	lastNUM8State = glfwGetKey(window, GLFW_KEY_DOWN);



	// changed to up arrow
	// "NUM 2" rotate counter clockwise around the x-axis
	
	if (lastNUM2State == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS
	//if (lastNUM2State == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS
		&& !((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			|| (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
		&& !keyDown && !keyPgUp && !keyPgDown && !keyLeft && !keyRight)
	{
		//rotatex += 90.0f;
		keyUp = true;

	}
	//lastNUM2State = glfwGetKey(window, GLFW_KEY_KP_2);
	lastNUM2State = glfwGetKey(window, GLFW_KEY_UP);


	// changed to page up
	// "NUM 4" rotate counter clockwise around the y-axis
	if (lastNUM4State == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS
	//if (lastNUM4State == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS
		&& !((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			|| (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
		&& !keyDown && !keyUp && !keyPgDown && !keyLeft && !keyRight)
	{
		//rotatey += 90.0f;
		keyPgUp = true;

	}
	//lastNUM4State = glfwGetKey(window, GLFW_KEY_KP_4);
	lastNUM4State = glfwGetKey(window, GLFW_KEY_PAGE_UP);

	// changed to page down
	// "NUM 6" rotate clockwise around the y-axis
	if (lastNUM6State == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS
	//if (lastNUM6State == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS
		&& !((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			|| (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
		&& !keyDown && !keyUp && !keyPgUp && !keyLeft && !keyRight)
	{
		//rotatey -= 90.0f;
		keyPgDown = true;

	}
	//lastNUM6State = glfwGetKey(window, GLFW_KEY_KP_6);
	lastNUM6State = glfwGetKey(window, GLFW_KEY_PAGE_DOWN);


	// changed to right arrow
	// "NUM 1" rotate counter clockwise around the z-axis
	if (lastNUM1State == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS
	//if (lastNUM1State == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS
		&& !((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			|| (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
		&& !keyDown && !keyPgUp && !keyPgDown && !keyLeft && !keyUp)
	{
		//rotatez += 90.0f;
		keyRight = true;

	}
	//lastNUM1State = glfwGetKey(window, GLFW_KEY_KP_1);
	lastNUM1State = glfwGetKey(window, GLFW_KEY_RIGHT);


	// changed to left arrow
	// "NUM 9" rotate clockwise around the z-axis
	if (lastNUM9State == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS
	//if (lastNUM9State == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS
		&& !((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			|| (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
		&& !keyDown && !keyPgUp && !keyPgDown && !keyUp && !keyRight)
	{
		//rotatez -= 90.0f;
		keyLeft = true; 

	}
	//lastNUM9State = glfwGetKey(window, GLFW_KEY_KP_9);
	lastNUM9State = glfwGetKey(window, GLFW_KEY_LEFT);



	// "KP_5 + Shift" fast movement forward (towards -z axis)
	if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)
	{
		//	gameSpeed = fastGameSpeed;
			// cameraPosition.z -= 0.2f;
	}





#pragma endregion

	// Scaling:
#pragma region Scaling Input

	// Scaling:

	// "U" scale up
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS
		&&
		((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			||
			(glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)))
	{
		scale += 0.01f;
	}

	// "J" scale down
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS
		&&
		((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			||
			(glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)))
	{
		scale -= 0.01f;
	}

#pragma endregion 


	// Space, move model back to initial position
#pragma region RESET MODEL

	if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)
	{
		/*	xPos = 0.0f;
			zPos = 0.0f;
			rotatex = 0.0f;
			rotatey = 0.0f;
			rotatez = 0.0f;
			scale = 1.0f;*/

	}
#pragma endregion


}

void input() {



	// added transitions
	if (rAngle <= 90.0f && keyLeft) {
		rAngle += angleRotateSpeed * dt;

		if (rAngle >= 90.0f) {
			rAngle = 0.0f;
			rz = 0.0f;
			rMatrix = rotate(mat4(1.0f), radians(90.0f), vec3(0.0f, 0.0f, 1.0f)) * rMatrix;
			keyLeft = false;
		}
		else {
			rz = rAngle;
		}
	}

	if (rAngle >= -90.0f && keyRight) {
		rAngle -= angleRotateSpeed * dt;

		if (rAngle <= -90.0f) {
			rAngle = 0.0f;
			rz = 0.0f;
			rMatrix = rotate(mat4(1.0f), radians(-90.0f), vec3(0.0f, 0.0f, 1.0f)) * rMatrix;
			keyRight = false;
		}
		else {
			rz = rAngle;
		}
	}

	if (rAngle <= 90.0f && keyDown) {
		rAngle += angleRotateSpeed * dt;

		if (rAngle >= 90.0f) {
			rAngle = 0.0f;
			rx = 0.0f;
			rMatrix = rotate(mat4(1.0f), radians(90.0f), vec3(1.0f, 0.0f, 0.0f)) * rMatrix;
			keyDown = false;
		}
		else {
			rx = rAngle;
		}
	}

	if (rAngle >= -90.0f && keyUp) {
		rAngle -= angleRotateSpeed * dt;

		if (rAngle <= -90.0f) {
			rAngle = 0.0f;
			rx = 0.0f;
			rMatrix = rotate(mat4(1.0f), radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)) * rMatrix;
			keyUp = false;
		}
		else {
			rx = rAngle;
		}
	}

	if (rAngle <= 90.0f && keyPgUp) {
		rAngle += angleRotateSpeed * dt;

		if (rAngle >= 90.0f) {
			rAngle = 0.0f;
			ry = 0.0f;
			rMatrix = rotate(mat4(1.0f), radians(90.0f), vec3(0.0f, 1.0f, 0.0f)) * rMatrix;
			keyPgUp = false;
		}
		else {
			ry = rAngle;
		}
	}

	if (rAngle >= -90.0f && keyPgDown) {
		rAngle -= angleRotateSpeed * dt;

		if (rAngle <= -90.0f) {
			rAngle = 0.0f;
			ry = 0.0f;
			rMatrix = rotate(mat4(1.0f), radians(-90.0f), vec3(0.0f, 1.0f, 0.0f)) * rMatrix;
			keyPgDown = false;
		}
		else {
			ry = rAngle;
		}
	}

	// send your varables to the selectModelMovement() function
	// the parameters are:
	// selectModelMovement( translation along x-axis, translation along z-axis, rotation around x-axis,
	//		rotation around y-axis, rotation around z-axis, scale);
	// 
	// the function is defined after the main loop if you want to see it

	//else if (select == 6.0) 
	//else;

	selectModelMovement(x, z, rx, ry, rz, s);
	// Model selection:
#pragma region MODEL SELECTION

// choose a number from 1-7
// also change the camera position to your model

//	select = 6.0;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		//	select = 0.0;
		selectMovement = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		//	select = 1.0;
		selectMovement = 1.0f;
	}

#pragma endregion

#pragma region SWITCH CAMERA

	//int selectedCamera = 1;


	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
		selectedCamera = 1;
		cameraPosition = defaultCameraPosition;
		cameraHorizontalAngle = -250.0f;
		cameraVerticalAngle = -5.0f;
	}

	// select Cai's model
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
		selectedCamera = 2;

		cameraPosition = vec3(49.0f, 49.0f, -49.0f);
		cameraHorizontalAngle = 225.0f;
		cameraVerticalAngle = -45.0f;

	}

	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {


	}


#pragma	endregion

	// Model movement parameters
#pragma region MODEL MOVEMENT PARAMETERS



#pragma endregion 

	// Camera movement
#pragma region CAMERA MOVEMENT

	// Turbo camera
	fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS
		||
		glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
	currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;


#pragma region CHARACTER MOVEMENT

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera down the z-axis
	{
		cameraPosition += currentCameraSpeed * dt * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera up the z-axis
	{
		cameraPosition -= currentCameraSpeed * dt * cameraFront;;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move camera to the left
	{
		cameraPosition -= normalize(cross(cameraFront, cameraUp)) * currentCameraSpeed * dt;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
	{
		cameraPosition += normalize(cross(cameraFront, cameraUp)) * currentCameraSpeed * dt;
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // move camera up
	{
		cameraPosition.y += currentCameraSpeed * dt;
	}

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) // move camera down
	{
		cameraPosition.y -= currentCameraSpeed * dt;
	}


#pragma endregion

#pragma endregion


	// Rendering:

#pragma region RENDER MODE
	// L for line
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// P for point
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	// T for triangle
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
#pragma endregion

#pragma region RENDER SWITCHIES

	// Toggle for texture

	if (lastXState == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && toggle == true)
	{
		toggle = false;
	}

	else if (lastXState == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		toggle = true;
	}
	lastXState = glfwGetKey(window, GLFW_KEY_X);


	// Toggle for shadow

	if (lastBState == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && shadowToggle == true)
	{
		shadowToggle = false;
	}

	else if (lastBState == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
	{
		shadowToggle = true;
	}
	lastBState = glfwGetKey(window, GLFW_KEY_B);


#pragma endregion

#pragma region WORLD ORIANTATION
	// world orientation
	/*
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		worldRotateY += 0.5;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		worldRotateNY += 0.5;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		worldRotateX += 0.5;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		worldRotateNX += 0.5;
	}
	*/
#pragma	endregion

#pragma region REST VIEW
	// reset the view
	if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS)
	{
		worldRotateX = 0.0;
		worldRotateNX = 0.0;
		worldRotateY = 0.0;
		worldRotateNY = 0.0;

		keyX = 0.0;
		keyY = 0.0;
		cameraHorizontalAngle = defaultCameraHorizontalAngle;
		cameraVerticalAngle = defaultCameraVerticalAngle;

		cameraPosition = defaultCameraPosition;
		//cameraFront = vec3(0.0f, 0.0f, -1.0f);
	}
#pragma endregion

#pragma region CAMERA CONTROL

	glfwGetCursorPos(window, &mousePosX, &mousePosY);

	dx = mousePosX - lastMousePosX;
	dy = mousePosY - lastMousePosY;

	lastMousePosX = mousePosX;
	lastMousePosY = mousePosY;



	// Use M to lock or release the mouse camera movement
	if (lastMState == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		if (cameraMouseLocked == false)
		{
			cameraMouseLocked = true;
		}
		else
		{
			cameraMouseLocked = false;
		}

	}
	lastMState = glfwGetKey(window, GLFW_KEY_M);

	if (cameraMouseLocked == false) {
		// tilt camera (y-axis)
		// Convert to spherical coordinates

		cameraVerticalAngle -= dy * cameraAngularSpeed * dt;

		// Clamp vertical angle to [-85, 85] degrees
		cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));


		//theta = 0;
		phi = radians(cameraVerticalAngle); // follow y axix

		cameraFront = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
		cameraSideVector = glm::cross(cameraFront, vec3(0.0f, 1.0f, 0.0f));

		glm::normalize(cameraSideVector);


		// Convert to spherical coordinates

		cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;

		if (cameraHorizontalAngle > 360)
		{
			cameraHorizontalAngle -= 360;
		}
		else if (cameraHorizontalAngle < -360)
		{
			cameraHorizontalAngle += 360;
		}

		theta = radians(cameraHorizontalAngle);
		//phi = 0;


		cameraFront = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));;
		cameraSideVector = glm::cross(cameraFront, vec3(0.0f, 1.0f, 0.0f));

		glm::normalize(cameraSideVector);
	}



	// tilt camera (y-axis)

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {

		// Convert to spherical coordinates

		cameraVerticalAngle -= dy * cameraAngularSpeed * dt;

		// Clamp vertical angle to [-85, 85] degrees
		cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));


		//theta = 0;
		phi = radians(cameraVerticalAngle); // follow y axix

		cameraFront = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
		cameraSideVector = glm::cross(cameraFront, vec3(0.0f, 1.0f, 0.0f));

		glm::normalize(cameraSideVector);
	}


	// pan camera (x-axis)
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		// Convert to spherical coordinates

		cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;

		if (cameraHorizontalAngle > 360)
		{
			cameraHorizontalAngle -= 360;
		}
		else if (cameraHorizontalAngle < -360)
		{
			cameraHorizontalAngle += 360;
		}

		theta = radians(cameraHorizontalAngle);
		//phi = 0;


		cameraFront = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));;
		cameraSideVector = glm::cross(cameraFront, vec3(0.0f, 1.0f, 0.0f));

		glm::normalize(cameraSideVector);

	}


	// zoom
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		//const float cameraAngularSpeed = 0.5f;
		//const float cameraAngularSpeed = 0.2f;
		cameraZoom -= dy * (0.2f) * dt;
		cameraSideVector = glm::cross(cameraFront, vec3(0.0f, 1.0f, 0.0f));

		glm::normalize(cameraSideVector);
	}



	lastMouseLeftState = GLFW_RELEASE;

	lastKeyX = keyX;
	lastKeyY = keyY;

	// Convert to spherical coordinates
	cameraHorizontalAngle -= dxKey * ((float)cameraAngularSpeed * 12.0f) * dt;
	cameraVerticalAngle -= dyKey * ((float)cameraAngularSpeed * 12) * dt;

	theta = radians(cameraHorizontalAngle);
	phi = radians(cameraVerticalAngle);

	cameraFront = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
	cameraSideVector = glm::cross(cameraFront, vec3(0.0f, 1.0f, 0.0f));

	glm::normalize(cameraSideVector);

#pragma endregion

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

GLboolean collisionAABB(vec3 one, vec3 two) // AABB - AABB collision
{
	float cubeSize = 0.05;
	// x axis collision
	bool collisionX = one.x + cubeSize > two.x &&
		two.x + cubeSize > one.x;
	// y axis collision
	bool collisionY = one.y + cubeSize > two.y &&
		two.y + cubeSize > one.y;

	bool collisionZ = one.z + cubeSize > two.z &&
		two.z + cubeSize > one.z;
	// only these 3 axis both  collision, then collision happen
	return collisionX && collisionY && collisionZ;
}

GLboolean checkCollision(vector<vec3> wallVec, vector<vec3> modelVec) {
	vector<vec3>::iterator it = wallVec.begin();
	// vector<int>::const_iterator iter=v.begin();
	for (; it != wallVec.end(); ++it)
	{
		vector<vec3>::iterator mit = modelVec.begin();
		for (; mit != modelVec.end(); ++mit) {

			if (collisionAABB(*it, *mit)) {

				cout << (*it).x << " " << (*mit).x << endl;
				cout << (*it).y << " " << (*mit).y << endl;
				cout << (*it).z << " " << (*mit).z << endl;

				cout << "collisioned" << endl;
				cout << "--------------------" << endl;
				return true;
			}
		}

	}
	return false;
}

void loadLevel(Shader shader, int mesh[3][3][3]) {



	//shuffle the model
	/*cout << rn1 << "\n";
	cout << rn3 << "\n";
	cout << rn2 << "\n";*/
	//mat4 shuffleRotateMat;


	// Movement func
	mat4 moveModelMat;

	if (selectMovement == 0) {

		moveModelMat = translate(mat4(1.0f), (vec3(6 * cos(alpha), 0.0f, 6.0f * sin(alpha))));

		alpha += 0.01f;
	}
	else
		moveModelMat = mat4(1.0f);




	mat4 baseModelOffse = glm::translate(mat4(1.0f), glm::vec3(0.0f, 6.0f, modelZposition));


	mat4 baseCubeScaleMat = scale(mat4(1.0f), vec3(baseCubeScale));

	mat4 moveBeroModel = glm::translate(mat4(1.0f), glm::vec3(x, y, z));

	mat4 rotateBeroModel =
		rotate(mat4(1.0f), radians(rx), vec3(1.0f, 0.0f, 0.0f))
		* rotate(mat4(1.0f), radians(ry), vec3(0.0f, 1.0f, 0.0f))
		* rotate(mat4(1.0f), radians(rz), vec3(0.0f, 0.0f, 1.0f));

	mat4 scaleBeroModelMat = glm::scale(mat4(1.0f), vec3(s));


	mat4 modelBase = baseModelOffse * moveModelMat * moveBeroModel * scaleBeroModelMat * rotateBeroModel * rMatrix * worldRotate;

	mat4 wallBase = baseModelOffse * worldRotate;



#pragma region Meshes

	// The indivedual cube location in each mesh
	mat4 offSetMat;

	// Move center of rotation from the corner to the center of the model
	mat4 modelCenterOffsetMat = translate(mat4(1.0f), glm::vec3(-1.0f, -1.0f, -1.0f));

	// Location of each mesh
	mat4 letterPositionMat;

	// mesh scale
	mat4 letterScaleMat = scale(mat4(1.0f), vec3(0.5f));

	// mesh matrix 
	mat4 meshMatrix;



	// Model
	// The color of the model
	vector<vec3> wallVec;
	vector<vec3> modelVec;
	//glBindVertexArray(wallCubeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, wallTextureID);
	shader.setVec3("objectColor", vec3(R, G, B));
	//shader.setVec3("objectColor", vec3(0.6627f, 0.1627f, 0.9627f));
	if (true) {


		for (int i = 0; i < 3; i++) {


			for (int j = 0; j < 3; j++) {

				for (int k = 0; k < 3; k++) {


					//cout << mesh1[i][j][k];
					if (mesh[j][k][i] == 1) {
						offSetMat = translate(mat4(1.0f), (vec3(0.0f + i, 0.0f + j, 0.0f + k)));

						meshMatrix =

							modelBase * letterScaleMat * modelCenterOffsetMat * offSetMat;

						shader.setMat4("model", meshMatrix);
						glDrawArrays(GL_TRIANGLES, 0, 36);
						vec4 position = meshMatrix * vec4(1.0f);
						modelVec.push_back(vec3(position.x, position.y, position.z));
					}
				}
			}
		}
	}


	glBindVertexArray(texturedCubeVAO);
	// Wall
	mat4 wallPositionMat = glm::translate(mat4(1.0f), glm::vec3(0.0f, 0.0f, wallZspace));

	// The color of the wall
	//shader.setVec3("objectColor", vec3(0.1627f, 0.8627f, 0.8627f));

	//shader.setInt("isTextured", 1);

	//glBindVertexArray(wallCubeVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, metalTextureID);

	if (true) {

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k <= 3; k++) {
					if (mesh[i][0][j] != 1 && mesh[i][1][j] != 1 && mesh[i][2][j] != 1)
					{
						offSetMat = translate(mat4(1.0f), (vec3(0.0f + i, 0.0f + j, 0.0f)));

						meshMatrix =

							wallBase * letterScaleMat * wallPositionMat * modelCenterOffsetMat * offSetMat;

						shader.setMat4("model", meshMatrix);
						glDrawArrays(GL_TRIANGLES, 0, 36);
						vec4 position = meshMatrix * vec4(1.0f);
						wallVec.push_back(vec3(position.x, position.y, position.z));

					}
				}
			}
		}
	}

	// wall filler

	int wallBuilder[7][7] = {


			{ 1, 1, 1, 1, 1, 1, 1},
			{ 1, 1, 1, 1, 1, 1, 1},
			{ 1, 1, 0, 0, 0, 1, 1},
			{ 1, 1, 0, 0, 0, 1, 1},
			{ 1, 1, 0, 0, 0, 1, 1},
			{ 1, 1, 1, 1, 1, 1, 1},
			{ 1, 1, 1, 1, 1, 1, 1}
	};


	

	mat4 wallBuilderCenterOffsetMat = translate(mat4(1.0f), glm::vec3(-3.0f, -3.0f, 0.0f));

	if (true) {

		for (int i = 0; i < 7; i++) {
			for (int j = 0; j < 7; j++) {

				if (wallBuilder[i][j] == 1)
				{
					offSetMat = translate(mat4(1.0f), (vec3(0.0f + i, 0.0f + j, -1.0f)));

					meshMatrix =

						wallBase * letterScaleMat * wallPositionMat * wallBuilderCenterOffsetMat * offSetMat;

					shader.setMat4("model", meshMatrix);
					glDrawArrays(GL_TRIANGLES, 0, 36);
					vec4 position = meshMatrix * vec4(1.0f);
					wallVec.push_back(vec3(position.x, position.y, position.z));
				}
			}
		}

		shader.setInt("isTextured", 0);

	}

	if (checkCollision(wallVec, modelVec)) {
		resetModel();
		//level++;
		//wallZspace += 3.0f;
		//gameSpeed += 0.005f;
		randomizeModelRotation();
		gameLogic->gainPoints(-50);
		gameLogic->nextLevel(0);
		randomizeColour();
		SoundEngine->play2D("transition.wav", GL_FALSE);
	}
	
		
	wallVec.clear();
	modelVec.clear();
}



void resetModel() {

	s = 1.0f;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	rx = 0.0f;
	ry = 0.0f;
	rz = 0.0f;

}


void resetModelPos() {

	x = 0.0f;
	y = 0.0f;
	z = 0.0f;

}

void moveModel(float xPos, float yPos, float zPos, float scale, float rotationX, float rotationY, float rotationZ) {

	s = 1.0f;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	rx = 0.0f;
	ry = 0.0f;
	rz = 0.0f;

}

void resetCamera() {

	cameraPosition = defaultCameraPosition;

	cameraHorizontalAngle = defaultCameraHorizontalAngle;
	cameraVerticalAngle = defaultCameraVerticalAngle;

}

void moveCamera(vec3 Position, float horizantalAng, float verticalAng) {

	cameraPosition = Position;

	cameraHorizontalAngle = horizantalAng;
	cameraVerticalAngle = verticalAng;

}


void randomizeModelRotation() {
	int minRange = 0;
	int maxRange = 10;
	int range = maxRange - minRange + 1;
	int rn1 = rand() % range + minRange;
	int rn2 = rand() % range + minRange;
	int rn3 = rand() % range + minRange;

	//rx = rn1 * 90;
	//ry = rn2 * 90;
	//rz = rn3 * 90;
	
	
	float rx1 = rn1 * 90;
	float ry2 = rn2 * 90;
	float rz3 = rn3 * 90;
 

	rMatrix = 
		rotate(mat4(1.0f), radians(rx1), vec3(1.0f, 0.0f, 0.0f)) 
		* rotate(mat4(1.0f), radians(ry2), vec3(0.0f, 1.0f, 0.0f))
		* rotate(mat4(1.0f), radians(rz3), vec3(0.0f, 0.0f, 1.0f))
		* rMatrix;	
	//cout << rx << "\n";
	//cout << ry << "\n";
	//cout << rz << "\n";
}


vec3 randomizeColourVec() {
	int minRange = 0;
	int maxRange = 10;
	int range = maxRange - minRange + 1;
	int R = (rand() % range + minRange) / 10;
	int G = (rand() % range + minRange) / 10;
	int B = (rand() % range + minRange) / 10;

	/*rx = rn1 * 90;
	ry = rn2 * 90;
	rz = rn3 * 90;*/

	//cout << "R" << R << "\n";
	//cout << "G" << G << "\n";
	//cout << "B" << B << "\n";

	return vec3(R, G, B);
}


void randomizeColour() {
	int minRange = 0;
	int maxRange = 10;
	int range = maxRange - minRange + 1;
	R = (rand() % range + minRange) / 10.0f;
	G = (rand() % range + minRange) / 10.0f;
	B = (rand() % range + minRange) / 10.0f;

	/*rx = rn1 * 90;
	ry = rn2 * 90;
	rz = rn3 * 90;*/

	//cout << "R" << R << "\n";
	//cout << "G" << G << "\n";
	//cout << "B" << B << "\n";


}
// randomizing model rotation

// zoom Using scroll
//
//if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
//{
//	if (fieldOfView >= 1.0f && fieldOfView <= 45.0f)
//	{
//		//const float cameraAngularSpeed = 0.5f;
//		//const float cameraAngularSpeed = 0.2f;
//		fieldOfView -= (0.2f) * dt;
//		cameraSideVector = glm::cross(cameraFront, vec3(0.0f, 1.0f, 0.0f));

//		glm::normalize(cameraSideVector);
//	}
//	if (fieldOfView <= 1.0f)
//	{
//		fieldOfView = 1.0f;
//	}
//	if (fieldOfView > 45.0f)
//	{
//		fieldOfView = 45.0f;
//	}
//}

//Cycling func	
//For the swap of texture on screen
	//timeOnTexture += dt;
	//if (timeOnTexture > 10) {
	//	timeOnTexture = 0;
	//	currentTexture += 1;
	//	if (currentTexture == 6) {
	//		currentTexture = 1;
	//	}
	//}

	// draw skybox as last
	//glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	//skyboxShader.use();
	//mat4 view = glm::mat4(glm::mat3(viewMatrix)); // remove translation from the view matrix


	//skyboxShader.setMat4("view", view);
	//skyboxShader.setMat4("projection", projectionMatrix);
	//// skybox cube
	//glBindVertexArray(skyboxVAO);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	////glDrawArrays(GL_TRIANGLES, 0, 36);
	//glBindVertexArray(0);
	//glDepthFunc(GL_LESS); // set depth function back to default


	//	// Testing the shadow map
	//debugDepthQuad.use();
	//debugDepthQuad.setFloat("near_plane", near_plane);
	//debugDepthQuad.setFloat("far_plane", far_plane);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, depthMap);
	//renderQuad();

GLuint setupModelVBO(string path, int& vertexCount) {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UVs;

	//read the vertex data from the model's OBJ file
	loadOBJ(path.c_str(), vertices, normals, UVs);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO); //Becomes active VAO
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).

	//Vertex VBO setup
	GLuint vertices_VBO;
	glGenBuffers(1, &vertices_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Normals VBO setup
	GLuint normals_VBO;
	glGenBuffers(1, &normals_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	//UVs VBO setup
	GLuint uvs_VBO;
	glGenBuffers(1, &uvs_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs, as we are using multiple VAOs)
	vertexCount = vertices.size();
	return VAO;
}

GLuint setupModelEBO(string path, int& vertexCount) {
	vector<int> vertexIndices;
	// The contiguous sets of three indices of vertices, normals and UVs, used to
	// make a triangle
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	vector<glm::vec2> UVs;

	// read the vertices from the cube.obj file
	// We won't be needing the normals or UVs for this program
	loadOBJ2(path.c_str(), vertexIndices, vertices, normals, UVs);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);  // Becomes active VAO
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and
	// attribute pointer(s).

	// Vertex VBO setup
	GLuint vertices_VBO;
	glGenBuffers(1, &vertices_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
		&vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
		(GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normals VBO setup
	GLuint normals_VBO;
	glGenBuffers(1, &normals_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3),
		&normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
		(GLvoid*)0);
	glEnableVertexAttribArray(1);

	// UVs VBO setup
	GLuint uvs_VBO;
	glGenBuffers(1, &uvs_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(),
		GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
		(GLvoid*)0);
	glEnableVertexAttribArray(2);

	// EBO setup
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(int),
		&vertexIndices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	// Unbind VAO (it's always a good thing to unbind any buffer/array to prevent
	// strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
	vertexCount = vertexIndices.size();
	return VAO;
}

void loadModel(Shader s) {
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, diffuseTextureID);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, diffuseGlowTextureID);


	glBindVertexArray(VAOT);
	mat4 ufo;

	s.setInt("isMaterial", 1);

	//s.setVec3("matAmbient", vec3(0.588235f, 0.588235f, 0.588235f)); 
	s.setVec3("matAmbient", vec3(0.7f, 0.7f, 0.7f));
	s.setVec3("matDiffuse", vec3(0.7f, 0.7f, 0.7f));
	//	s.setVec3("matDiffuse", vec3(0.588235f, 0.588235f, 0.588235f));
	s.setVec3("matSpecualr", vec3(0.000000f, 0.000000f, 0.000000f));
	s.setFloat("matShine", 9.999999);


	float posX = sin(glfwGetTime()) * 1.0f;
	float posZ = cos(glfwGetTime()) * 1.0f;

	ufo = translate(mat4(1.0f), vec3(posX, 5.0f, posZ)) * scale(mat4(1.0f), vec3(0.1f, 0.1f, 0.1f));
	// ufo = translate(mat4(1.0f), vec3(5.0f, 5.0f, 5.0f)) * scale(mat4(1.0f), vec3(0.1f, 0.1f, 0.1f));
	//ufo = scale(mat4(1.0f), vec3(0.1f, 0.1f, 0.1f));

	s.setMat4("model", ufo);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);


	s.setInt("isMaterial", 0);

	s.setVec3("matAmbient", initialAmbient);
	s.setVec3("matDiffuse", initialDiffuse);
	s.setVec3("matSpecualr", initialSpecular);
	s.setFloat("matShine", initialShine);
}

void loadModel2(Shader s) {

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, diffuseTextureID);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, diffuseGlowTextureID);


	glBindVertexArray(VAOT2);
	mat4 ufo;

	s.setInt("isMaterial", 1);

	//s.setVec3("matAmbient", vec3(0.588235f, 0.588235f, 0.588235f)); 
	s.setVec3("matAmbient", vec3(0.7f, 0.7f, 0.7f));
	s.setVec3("matDiffuse", vec3(0.7f, 0.7f, 0.7f));
	//	s.setVec3("matDiffuse", vec3(0.588235f, 0.588235f, 0.588235f));
	s.setVec3("matSpecualr", vec3(0.000000f, 0.000000f, 0.000000f));
	s.setFloat("matShine", 9.999999);


	float posX = sin(glfwGetTime()) * 30.0f;
	float posZ = cos(glfwGetTime()) * 30.0f;

	mat4 moveModelMat = translate(mat4(1.0f), (vec3(6 * cos(alpha), 0.0f, 6.0f * sin(alpha))));

	alpha += 0.001f;

	ufo = translate(mat4(1.0f), vec3(posX*3, 5.0f, posZ*3 +12)) * scale(mat4(1.0f), vec3(0.1f, 0.1f, 0.1f)) * moveModelMat;
	 //ufo = translate(mat4(1.0f), vec3(5.0f, 5.0f, 5.0f)) * scale(mat4(1.0f), vec3(0.1f, 0.1f, 0.1f));
	//ufo = scale(mat4(1.0f), vec3(0.1f, 0.1f, 0.1f));

	s.setMat4("model", ufo);



	glDrawArrays(GL_TRIANGLES, 0, vertexCount);



	s.setInt("isMaterial", 0);

	s.setVec3("matAmbient", initialAmbient);
	s.setVec3("matDiffuse", initialDiffuse);
	s.setVec3("matSpecualr", initialSpecular);
	s.setFloat("matShine", initialShine);
}

