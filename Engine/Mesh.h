#pragma once
#define GLEW_STATIC 
#include <glew.h>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include "Texture.h"

struct Vertex{
	glm::vec3 position;
	glm::vec2 texture_uv;
	int bone_ids[4];
	float weights[4];
};

class Mesh
{
public:
	Mesh(const std::vector<Vertex>& vertices,const std::vector<unsigned int>& indices,std::vector<Texture>& textures);
	~Mesh();
	void Draw();
private:
	int m_IndicesCount = 0;
	unsigned int m_Vao = 0;
	std::vector<Texture> m_Textures;
};