/* esse arquivo possui a implementacao das funcoes declaradas no arquivo Utils.h
com finalidade de uso em programas graficos usando OpenGL e arquivos .glsl*/

/* repare que o uso deve ser exclusivo para programas com include de "GL/glew", "GLFW/glfw3", "string" */

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL2/soil2.h>
#include <string>
#include <fstream>
#include <iostream>
#include <windows.h>
#include "OpenGL_Utils.h"

//MACROS
#define SHADER_LOAD_ERROR  "SHADER_LOAD_ERROR"



// Definindo constantes para cores para prints
// std::string RESET = "\x1b[0m";
//const std::string RED = "\x1b[31m";
//const std::string GREEN = "\x1b[32m";
//const std::string YELLOW = "\x1b[33m";
//const std::string BLUE = "\x1b[34m";
//const std::string MAGENTA = "\x1b[35m";
//const std::string CYAN = "\x1b[36m";

using namespace std;

void setConsoleColor(WORD color) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}

void resetConsoleColor() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void printError(const char* texto, const char* funcName) {
	if (funcName != "") {
		setConsoleColor(FOREGROUND_RED | FOREGROUND_BLUE); // Roxo
		cout << "("<<funcName<<") ";
	}
	setConsoleColor(FOREGROUND_RED); // Vermelho
	cout << "Error: ";
	resetConsoleColor();
	cout << texto << endl;
}

void printColorido(COLOR cor, const char* texto, const char* funcName) {
	if (funcName != "") {
		setConsoleColor(FOREGROUND_RED | FOREGROUND_BLUE); // Roxo
		cout << "(" << funcName << ") ";
	}
	if (cor == RED)
		setConsoleColor(FOREGROUND_RED); // Vermelho
	else if (cor == BLUE)
		setConsoleColor(FOREGROUND_BLUE);
	else if (cor == GREEN)
		setConsoleColor(FOREGROUND_GREEN);
	else if (cor == CYAN)
		setConsoleColor(FOREGROUND_BLUE | FOREGROUND_GREEN);
	else if (cor == PURPLE)
		setConsoleColor(FOREGROUND_BLUE | FOREGROUND_RED);
	else
		setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN);
	cout << "INFO: ";
	resetConsoleColor();
	cout << texto << endl;
}

void printShaderLog(GLuint shader) { //1
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetShaderInfoLog(shader, len, &chWrittn, log);
		setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN); // Amarelo
		cout << "Shader Info Log: ";
		resetConsoleColor();
		cout << "\n" << log << endl;
		free(log);
	}
} //1

void printProgramLog(int prog) { //2
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetProgramInfoLog(prog, len, &chWrittn, log);
		cout << "Program Info Log: " << "\n" << log << endl;
		free(log);
	}
} //2

bool checkOpenGLError() { //3
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR) {
		cout << "glError: " << glErr << endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
} //3

string readShaderSource(const char* filePath, bool debug) { //4
	string content;
	if (debug) { 
		setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN); // Amarelo
		cout << "Loading Shader: " << filePath;
		resetConsoleColor();
	}
	try {
		ifstream fileStream(filePath, ios::in);
		if (!fileStream.is_open()) {
			throw runtime_error(" Could not open file");
		}

		string line;
		while (getline(fileStream, line)) {
			content.append(line + "\n");
		}
		fileStream.close();
		if (debug) { 
			setConsoleColor(FOREGROUND_GREEN); // Verde
			cout << " OK " << endl;
			resetConsoleColor();
		}
		return content;
	}
	catch (exception& e) {
		setConsoleColor(FOREGROUND_RED); // Vermelho
		cout << " FAILURE: " << e.what() << endl;
		resetConsoleColor();
	}
	return SHADER_LOAD_ERROR;
}

GLuint createShaderProgram(const char* v_ShaderSource_path, const char* f_ShaderSource_path, bool debug) { //5
	GLint vertCompiled;
	GLint fragCompiled;
	GLint linked;

	string vertShaderStr = readShaderSource(v_ShaderSource_path, debug);
	string fragShaderStr = readShaderSource(f_ShaderSource_path, debug);

	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	if (debug) { cout << "Compiling Vertex Shader: "; }

	glCompileShader(vShader);//verificando erros de compilacao do vertex_ShaderSource
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);

	if (vertCompiled != 1) {
		setConsoleColor(FOREGROUND_RED);
		cout << "vertex compilation failed" << endl;
		resetConsoleColor();
		printShaderLog(vShader);
	}
	else {
		if (debug) { 
			setConsoleColor(FOREGROUND_GREEN);
			cout << "OK" << endl;
			resetConsoleColor();
		}
	}

	if (debug) { cout << "Compiling Fragment Shader: "; }

	glCompileShader(fShader);//verificando erros de compilacao do vertex_ShaderSource
	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		setConsoleColor(FOREGROUND_RED);
		cout << "fragment compilation failed" << endl;
		printShaderLog(fShader);
	}
	else {
		if (debug) { 
			setConsoleColor(FOREGROUND_GREEN);
			cout << "OK" << endl;
			resetConsoleColor();
		}
	}

	GLuint vfProgram = glCreateProgram();

	// catch errors while linking shaders
	if (debug) { cout << "Linking Vertex and Fragment Shaders: "; }
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);
	glLinkProgram(vfProgram);
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		setConsoleColor(FOREGROUND_RED);
		cout << "linking failed" << endl;
		resetConsoleColor();
		printProgramLog(vfProgram);
	}
	if (debug) {
		if (linked != 1) {
			setConsoleColor(FOREGROUND_RED);
			cout << "Could not make the OpenGL program." << endl;
			resetConsoleColor();
		}
		else {
			setConsoleColor(FOREGROUND_GREEN);
			cout << "OK" << endl;
			//setConsoleColor(FOREGROUND_GREEN);
			cout << "OpenGL program made with sucess.\n" << endl;
			resetConsoleColor();
		}
	}
	glDeleteShader(vShader);//para evitar "saves" de shaders na GPU
	glDeleteShader(fShader);//pelo menos deveria

	return vfProgram;
} //5

GLuint loadTexture(const char* texImagePath) {
	GLuint textureID;
	textureID = SOIL_load_OGL_texture(texImagePath,
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (textureID == 0) {
		setConsoleColor(FOREGROUND_RED);
		cout << "ERROR: load image";
		resetConsoleColor();
		cout << " - could not find texture file ";
		setConsoleColor(FOREGROUND_RED);
		cout << texImagePath << endl;
		resetConsoleColor();
	}
	return textureID;
}

GLuint loadCubeMap(const char* mapDir) {
	GLuint textureRef;
	// assumes that the six texture image files are named xp, xn, yp, yn, zp, zn and are JPG
	string xp = mapDir; xp = xp + "/xp.jpg";
	string xn = mapDir; xn = xn + "/xn.jpg";
	string yp = mapDir; yp = yp + "/yp.jpg";
	string yn = mapDir; yn = yn + "/yn.jpg";
	string zp = mapDir; zp = zp + "/zp.jpg";
	string zn = mapDir; zn = zn + "/zn.jpg";
	textureRef = SOIL_load_OGL_cubemap(xp.c_str(), xn.c_str(), yp.c_str(), yn.c_str(),
		zp.c_str(), zn.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (textureRef == 0) {
		string xp = mapDir; xp = xp + "/xp.png";
		string xn = mapDir; xn = xn + "/xn.png";
		string yp = mapDir; yp = yp + "/yp.png";
		string yn = mapDir; yn = yn + "/yn.png";
		string zp = mapDir; zp = zp + "/zp.png";
		string zn = mapDir; zn = zn + "/zn.png";
		textureRef = SOIL_load_OGL_cubemap(xp.c_str(), xn.c_str(), yp.c_str(), yn.c_str(),
			zp.c_str(), zn.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	}
	if (textureRef == 0)
		printError("O arquivo de cube map nao foi encontrado","OpenGL_Utils::loadCubeMap");

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureRef);
	// reduce seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return textureRef;
}