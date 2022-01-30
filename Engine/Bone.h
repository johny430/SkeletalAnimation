#pragma once
#include <glm/glm.hpp>

class Bone
{
public:
	Bone(int id,glm::mat4& offset_matrix);
	~Bone();

public:
	int m_ID = -1;
	glm::mat4 m_LocalSpaceToBone = glm::mat4(1.f);
	friend class Animation;
	friend struct BoneData;
};