#pragma once
#include <iostream>
#define GLEW_STATIC 
#include <glew.h>
#include <string>
#include <stb_image.h>

class Texture
{
public:
	Texture(const std::string& path);
	~Texture();

	void Bind() const;
private:
	unsigned int m_ID;
};