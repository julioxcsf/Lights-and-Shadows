/* esse arquivo contem os cabeçalhos e macros mais comuns
que podem ser utilizados nos varios programas graficos de OpenGL */

/* repare que o uso deve ser exclusivo para programas com include de "GL/glew", "GLFW/glfw3", "string" */

#ifndef UTILS_H
#define UTILS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <windows.h>
#include <string>

enum COLOR {
	RED,
	BLUE,
	GREEN,
	CYAN,
	PURPLE,
	YELLOW
};

/* PADRAO -----------------------------------------
tipo nome_fucao(tipo_arguemnto); //numero_funcao */

void setConsoleColor(WORD);

void resetConsoleColor();

void printError(const char* text, const char* funcName = "");

void printColorido(COLOR cor, const char* texto, const char* funcName = "");

void printShaderLog(GLuint);

void printProgramLog(int);

bool checkOpenGLError();

std::string readShaderSource(const char*, bool);

GLuint createShaderProgram(const char*, const char*, bool);

GLuint loadTexture(const char* texImagePath);

GLuint loadCubeMap(const char* mapDir);

#endif // UTILS_H