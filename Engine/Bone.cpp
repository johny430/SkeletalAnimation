#include "Bone.h"

Bone::Bone(int id, glm::mat4& offset_matrix)
	: m_ID(id), m_LocalSpaceToBone(offset_matrix)
{
}

Bone::~Bone()
{
}
