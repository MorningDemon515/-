#pragma once
#ifndef GRAPHICS
#define GRAPHICS

#include "Main.h"

#include <glad/glad.h>

class Graphics
{
public:
	Graphics(HWND hwnd);
	~Graphics();

	int Draw();

private:
	GLuint VAO, VBO,EBO,texture;

	int width, height, nrChannels;

};


#endif 


