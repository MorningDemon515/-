#pragma once
#ifndef GRAPHICS
#define GRAPHICS

#include "Main.h"

#include <glad/glad.h>

#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H  

class Graphics
{
public:
	Graphics(HWND hwnd);
	~Graphics();

	int Draw();

	void RenderText( std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

private:
	GLuint VAO, VBO,EBO,texture;

	int width, height, nrChannels;

	/// Holds all state information relevant to a character as loaded using FreeType
	struct Character {
		GLuint TextureID;   // ID handle of the glyph texture
		glm::ivec2 Size;    // Size of glyph
		glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
		GLuint Advance;    // Horizontal offset to advance to next glyph
	};

	std::map<GLchar, Character> Characters;

};


#endif 


