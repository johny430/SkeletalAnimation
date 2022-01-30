#include "Animation.h"

Animation::Animation(std::vector<AnimationNode>&& nodes, float duration, float tick_per_second)
	: m_Nodes(nodes), m_Duration(duration), m_TicksPerSecond(tick_per_second), m_Transforms(100)
{
    for (int i = 0; i < 100; i++)
    {
        m_Transforms[i] = glm::mat4(1.0f);
    }
}

void Animation::Update(float dt)
{
    m_CurrentTime += m_TicksPerSecond * dt;
    m_CurrentTime = fmod(m_CurrentTime, m_Duration);

    for (std::size_t i = 0, count = m_Nodes.size(); i < count; ++i)
    {
        AnimationNode& node = m_Nodes[i];

        const glm::mat4 bone_transform = node.has_key_frames
            ? node.frames.InterpolateFramesAt(m_CurrentTime)
            : node.node_transform;
        const glm::mat4 parent_transform = (node.parent >= 0)
            ? m_Nodes[node.parent].bone_transform
            : glm::mat4(1.0f);
        const glm::mat4 final_ = parent_transform * bone_transform;
        node.bone_transform = final_;

        if (node.has_key_frames)
        {
            m_Transforms[node.bone->m_ID] = final_ * node.bone->m_LocalSpaceToBone;
        }
    }
}
