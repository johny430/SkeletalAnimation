#pragma once
#include "Bone.h"
#include "Mesh.h"
#include "Shader.h"
#include <span>

class Model
{
public:
	Model(std::vector<Mesh>&& meshes, std::vector<Bone>&& bones);
	~Model();

	void Draw(const glm::mat4& projection,const glm::mat4& view,const glm::mat4& model, std::span<const glm::mat4> transforms);
private:
	std::vector<Bone> m_Bones;
	std::vector<Mesh> m_Meshes;
	Shader m_Shader;
	int m_ProjectionPtr = -1;
	int m_ViewPtr		= -1;
	int m_ModelPtr		= -1;
	int m_TransformsPtr = -1;
};