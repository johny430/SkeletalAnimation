#pragma once
#include "Bone.h"
#include "KeyFrames.h"
#include <span>


struct AnimationNode
{
	KeyFrames frames;
	Bone* bone = nullptr;
	bool  has_key_frames = false;
	//nodes_final_global_transform
	glm::mat4 bone_transform;

	// Constant data.
	int parent = -1;
	glm::mat4 node_transform;
};

class Animation
{
public:
	Animation(std::vector<AnimationNode>&& nodes, float duration, float tick_per_second);
	~Animation() {};

	void Update(float dt);

	std::span<const glm::mat4> transforms() const
	{
		return std::span<const glm::mat4>(m_Transforms.cbegin(), 52);
	}
public:
	std::vector<AnimationNode> m_Nodes;
	std::vector<glm::mat4>   m_Transforms;
	float m_CurrentTime = 0.0f;
	float m_Duration;
	float m_TicksPerSecond;
};